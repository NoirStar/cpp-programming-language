#pragma once
#include <functional>

class ThreadPool {
public:
	explicit ThreadPool(size_t num_threads);
	virtual ~ThreadPool();

	void WorkerThread();

	// job ��� Ÿ���� functor�� �ް� ���ϰ��� ������ future �� ����
	// F�� funtor, �ι�°�� ��������
	template<typename F, typename... Args>
	std::future<typename std::result_of<F(Args...)>::type>
		EnqueueJob(F&& f, Args&&... args);

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

private:
	size_t num_threads_;
	std::vector<std::thread> worker_threads_;
	std::queue<std::function<void()>> jobs_;
	std::condition_variable cv_job_q_;
	std::mutex m_job_q_;
	bool stop_all_;

};

