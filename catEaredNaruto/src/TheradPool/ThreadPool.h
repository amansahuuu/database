

#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <thread>
#include <functional>

class ThreadPool {
public:
	explicit ThreadPool(size_t numThreads);
	~ThreadPool();
	void enqueue (std::function <void()> task);
private:
	std::queue<std::function <void()>> m_tasks; //for tasks got by user
	std::mutex m;
	std::condition_variable cv;
	std::vector<std::thread> m_threads; //for 8 threads
	bool m_stop = false;
};