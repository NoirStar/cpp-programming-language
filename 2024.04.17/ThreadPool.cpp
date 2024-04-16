#include "pch.h"
#include "ThreadPool.h"


ThreadPool::ThreadPool(size_t num_threads)
	: num_threads_(num_threads), stop_all_(false) {
	worker_threads_.reserve(num_threads_);
	for (size_t i = 0; i < num_threads_; ++i) {
		worker_threads_.push_back(std::thread([this]() {this->WorkerThread(); }));
	}
}

ThreadPool::~ThreadPool() {
	stop_all_ = true;
	cv_job_q_.notify_all();

	for (auto& t : worker_threads_) {
		// join�� ȣ�⾲���忡�� �������� �����尡 �Ϸ�ɶ����� ��ٸ��°�
		t.join();
	}
}


void ThreadPool::WorkerThread() {
	while (true) {
		std::unique_lock<std::mutex> lock(m_job_q_);
		// conditioal variable �� ��� ť�� ���� ���� ������ �����ؾ��ϰ�
		// stop_all �� �־ �׸� ��ٷ��� �ߴ��ؾ��Ѵ�.
		cv_job_q_.wait(lock, [this]() {return !this->jobs_.empty() && stop_all_; });
		if (stop_all_ && this->jobs_.empty()) return;

		// ���� ������ �ϳ� ���� �����Ѵ�.
		std::function<void()> job = std::move(jobs_.front());
		jobs_.pop();

		lock.unlock();
		// ���� �����Ѵ�.
		job();
	}
}

template<typename F, typename... Args>
std::future<typename std::result_of<F(Args...)>::type>
ThreadPool::EnqueueJob(F&& f, Args&&... args) {
	if (stop_all_) {
		throw std::runtime_error("ThreadPool ��� ������");
	}
	// typename�� Ÿ���̶��� �˷��ִ� Ű����
	using return_type = typename std::result_of<F(args...)>::type;

	// job �� ���� ���ϰ��� �ޱ����ؼ���.. �񵿱������� ����Ǵ�
	// std::queue<std::function<void()>> jobs_; �� �߰��ؾ���

	// �񵿱��Լ��� ���ϰ��� �ޱ�����, future�� �����ϴ� packaged_task�� ĸ��ȭ
	// ���ڷδ� �Լ� ��ü�� �ް�, bind�� �̿��Ͽ� �Լ��� ���ڸ� ��������ش�.

	auto job = std::make_shared<std::packaged_task<return_type()>>(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	
	// future�� ����(�̷��� ���ϰ��� �����ְڴٴ� ���)
	std::future<return_type> job_result_future = job->get_future();
	{
		std::lock_guard<std::mutex> lock(m_job_q_);

		// void() �� �̱� ������ �ѹ��� �����ش�.
		jobs_.push([job]() {(*job)(); });
	}
	cv_job_q_.notify_one();

	return job_result_future;
}

