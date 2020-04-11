#pragma once

#include <functional>
#include <atomic>
#include <mutex>
#include <chrono>

namespace Prism {

	using TaskFunction = std::function<void(uint32_t)>;

	/**
	 * (Hopefully) thread-safe ringbuffer implementation
	 */
	template<uint32_t CAPACITY>
	class TaskQueue {
	public:

		bool full() { return (head + 1) % CAPACITY == tail.load(); }
		bool empty() { return head.load() == tail.load(); }

		bool push(const TaskFunction& f)
		{
			std::unique_lock<std::mutex> lock;
			uint32_t current;

			// sets current according to state of the queue
			// returns if slot available & locked
			auto update = [&]()
			{
				current = head.load();
				if (full()) return false;

				if (lock.owns_lock()) lock.unlock();
				lock = std::unique_lock<std::mutex>(taskLocks[current], std::try_to_lock);
				return true;
			};

			if (!update()) return false; // initialize
			while (!head.compare_exchange_weak(current, (current + 1) % CAPACITY))
				if (!update()) return false;

			// head is modified, but the element is still locked

			tasks[current] = f;
			return true;
		}

		bool pop(TaskFunction& f) 
		{
			std::unique_lock<std::mutex> lock;
			uint32_t current;

			// sets current according to state of the queue
			// returns if element available & locked
			auto update = [&]()
			{
				current = tail.load();
				if (empty()) return false;

				if (lock.owns_lock()) lock.unlock();
				lock = std::unique_lock<std::mutex>(taskLocks[current], std::try_to_lock);
				return true;
			};

			if (!update()) return false; // initialize
			while (!tail.compare_exchange_weak(current, (current + 1) % CAPACITY))
				if (!update()) return false;

			// tail is modified, but the element is still locked

			f = tasks[current];
			return true;
		}


	private:
		std::atomic<uint32_t> head = 0;
		std::atomic<uint32_t> tail = 0;

		TaskFunction tasks[CAPACITY];
		std::mutex taskLocks[CAPACITY];
	};
}