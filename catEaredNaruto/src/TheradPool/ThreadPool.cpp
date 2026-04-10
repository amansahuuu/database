
#include "ThreadPool.h"
ThreadPool::ThreadPool(size_t numThreads)
{
	for (int i = 0; i < numThreads; i++) {
		m_threads.emplace_back([this]() {
			while (true) {
				std::function <void()> task;
				{
					std::unique_lock <std::mutex> lock(m);
					cv.wait(lock, [this] { return m_stop || !m_tasks.empty();});
					if (m_stop && m_tasks.empty()) { return; };
					task = m_tasks.front();
					m_tasks.pop();
				}
				task();
			}
			});
	}
}

ThreadPool::~ThreadPool()
{
	
	{
		std::unique_lock <std::mutex> lock(m);
		m_stop = true;
	}
	cv.notify_all();

	for (std::thread& thr : m_threads) {
		if (thr.joinable()) {
			thr.join();
		}
	}
}

void ThreadPool::enqueue(std::function<void()> task)
{
	std::unique_lock<std::mutex> lock(m);
	m_tasks.push(task);
	cv.notify_one();
}
