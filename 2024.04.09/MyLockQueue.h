#pragma once

#include <mutex>

template<typename T>
class MyLockQueue {
public:
	MyLockQueue() {}
	virtual ~MyLockQueue() {}

	MyLockQueue(const MyLockQueue&) = delete;
	MyLockQueue& operator=(const MyLockQueue&) = delete;

	void TryPush(T arg) {
		std::lock_guard<std::mutex> lock(mutex_);
		queue_.push(std::move(arg));
		cond_.notify_one();
	}

	bool TryPop(T& arg) {
		if (queue_.empty()) {
			return false;
		}
		arg = std::move(queue_.front());
		queue_.pop();
		return true;
	}

	void WaitPop(T& arg) {
		std::unique_lock<std::mutex> lock(mutex_);
		cond_.wait(lock, [this]() { return queue_.empty() == false});
		arg = std::move(queue_.front());
		queue_.pop();
	}

private:
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;
};

