#include "TaskSystem.h"
#include "TaskQueue.h"

#include "Util/Log/Log.h"

#include <atomic>

// uncomment for detailed logging
#define PR_THREAD_TRACE(...) //PR_CORE_TRACE(__VA_ARGS__)

namespace Prism {

	Prism::TaskQueue<256> g_Queue;

	uint32_t g_NumThreads;
	std::vector<std::thread> g_Threads;
	std::mutex g_SignalMutex;
	std::condition_variable g_SignalCondition;

	// latest queued task
	std::atomic<uint64_t> g_QueueIndex;
	// latest finished task
	std::atomic<uint64_t> g_ExecutionIndex;
	// if Finish() has been called
	bool g_Finished = false;

	void processTasks(uint32_t id)
	{
		TaskFunction task;

		while (true)
		{
			if (g_Queue.pop(task))
			{
				PR_THREAD_TRACE("worker-{0} starting task", id);
				task(id);

				// increment executionIndex
				uint64_t current = g_ExecutionIndex.load();
				while (g_ExecutionIndex.compare_exchange_strong(current, current + 1));
				PR_THREAD_TRACE("worker-{0} finished task, executionIndex={1}", id, current);
			}
			else if (!g_Finished)
			{
				// no task left, wait for signal
				std::unique_lock<std::mutex> lock(g_SignalMutex);
				while (g_Queue.empty() && !g_Finished)
					g_SignalCondition.wait(lock);
			}
			else
			{
				// no task left, should finish
				break;
			}
		}
	}

	Task::Task(std::function<void()> fun, bool lockable)
		: mFunction(std::move(fun))
	{
		if (lockable) mLock = std::make_shared<TaskLock>();
	}

	void TaskSystem::Init()
	{
		g_NumThreads = std::thread::hardware_concurrency() - 1;
		g_Threads = std::vector<std::thread>(g_NumThreads);
		g_QueueIndex = g_ExecutionIndex = 0;

		PR_CORE_INFO("Starting TaskSystem with {0} worker threads", g_NumThreads);
		PR_CORE_ASSERT(g_NumThreads > 0, "There are no worker threads, tasks will be executed on main");

		for (uint32_t i = 0; i < g_NumThreads; ++i)
			g_Threads[i] = std::thread([=]() { processTasks(i); });
	}

	void TaskSystem::Submit(const Task& task)
	{
		if (g_NumThreads > 0)
		{
			// increment queueIndex
			uint64_t current = g_QueueIndex.load();
			while (g_QueueIndex.compare_exchange_strong(current, current + 1));

			PR_THREAD_TRACE("Submitting task, queueIndex={0}", current);

			std::function<void(uint32_t)> function;

			if (!task.mLock) function = [f = task.mFunction](uint32_t) { f(); };
			else function = [f = task.mFunction, lock = std::weak_ptr(task.mLock)](uint32_t) {
				f(); if (auto l = lock.lock()) l->notify();
			};

			bool success = g_Queue.push(std::move(function));
			PR_CORE_ASSERT(success, "Could not push task!");
			g_SignalCondition.notify_one();
		}
		else
		{
			// if there are no worker threads, handle on main thread
			// blocking here until all recursive tasks are complete
			task.mFunction();
		}
	}

	void TaskSystem::Wait()
	{
		while (g_ExecutionIndex.load() < g_QueueIndex.load())
		{
			g_SignalCondition.notify_one();
			std::this_thread::yield(); // allow this thread to be rescheduled
		}
	}

	void TaskSystem::Finish()
	{
		g_Finished = true;
		g_SignalCondition.notify_all();
		for (auto& thread : g_Threads)
			thread.join();
	}
}