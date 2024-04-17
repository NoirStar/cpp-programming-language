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
		// join은 호출쓰레드에서 실행중인 쓰레드가 완료될때까지 기다리는것
		t.join();
	}
}


void ThreadPool::WorkerThread() {
	while (true) {
		std::unique_lock<std::mutex> lock(m_job_q_);
		// conditioal variable 로 대기 큐에 뭔가 값이 있으면 진행해야하고
		// stop_all 이 있어도 그만 기다려서 중단해야한다.
		cv_job_q_.wait(lock, [this]() {return !this->jobs_.empty() || stop_all_; });
		if (stop_all_ && this->jobs_.empty()) return;

		// 잡이 있으면 하나 빼서 수행한다.
		std::function<void()> job = std::move(jobs_.front());
		jobs_.pop();

		lock.unlock();
		// 잡을 수행한다.
		job();
	}
}

template<typename F, typename... Args>
std::future<typename std::result_of<F(Args...)>::type>
ThreadPool::EnqueueJob(F&& f, Args&&... args) {
	if (stop_all_) {
		throw std::runtime_error("ThreadPool 사용 중지됨");
	}
	// typename이 타입이란걸 알려주는 키워드
	using return_type = typename std::result_of<F(args...)>::type;

	// job 에 대한 리턴값을 받기위해서는.. 비동기적으로 실행되는
	// std::queue<std::function<void()>> jobs_; 에 추가해야함

	// 비동기함수의 리턴값을 받기위해, future를 리턴하는 packaged_task로 캡슐화
	// 인자로는 함수 객체를 받고, bind를 이용하여 함수와 인자를 연결시켜준다.

	auto job = std::make_shared<std::packaged_task<return_type()>>(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	
	// future를 저장(미래에 리턴값을 돌려주겠다는 약속)
	std::future<return_type> job_result_future = job->get_future();
	{
		std::lock_guard<std::mutex> lock(m_job_q_);

		// void() 형 이기 때문에 한번더 감싸준다.
		jobs_.push([job]() {(*job)(); });
	}
	cv_job_q_.notify_one();

	return job_result_future;
}

