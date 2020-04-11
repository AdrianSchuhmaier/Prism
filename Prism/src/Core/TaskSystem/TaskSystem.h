#pragma once

#include "Util/Log/Log.h"

#include <functional>
#include <thread>
#include <atomic>
#include <vector>

namespace Prism {

	struct Task; // defined below
	struct TaskLock; // defined below

	class TaskSystem {
	public:
		static void Init();
		static void Submit(const Task& task);
		static void Wait();
		static void Finish();
	};

	struct TaskLock {
		TaskLock() : lock(mutex) {}
		TaskLock(const TaskLock&) = delete;
		TaskLock& operator=(const TaskLock&) = delete;

		// if the task is completed
		inline bool finished() { return m_Finished.load(); }
		// wait for the task to complete
		inline void wait() { while (!m_Finished.load()) cv.wait(lock); }
		// notify that the task is completed
		inline void notify() { m_Finished.store(true); cv.notify_all(); }
	private:
		std::atomic<bool> m_Finished = false;
		std::mutex mutex;
		std::condition_variable cv;
		std::unique_lock<std::mutex> lock;
	};

	struct Task {
		friend class TaskSystem;

		Task(std::function<void()> fun, bool lockable = false);

		void Submit() { TaskSystem::Submit(*this); }
		void Wait() { if (mLock) mLock->wait(); else PR_CORE_WARN("Cannot wait for non-existent lock"); }

	private:
		std::function<void()> mFunction;
		std::shared_ptr<TaskLock> mLock = nullptr;
	};
}