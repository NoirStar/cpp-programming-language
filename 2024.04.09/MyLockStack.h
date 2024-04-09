#pragma once

#include <mutex>

template<typename T>
class MyLockStack {
public:
	MyLockStack() {};
	~MyLockStack() {};

	MyLockStack(const MyLockStack&) = delete;
	MyLockStack& operator=(const MyLockStack&) = delete;

	void Push(T arg) {
		std::lock_guard<std::mutex> lock(mutex_);
		stack_.push(std::move(arg));
	}
	bool TryPop(T& arg) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (stack_.empty()) {
			return false;
		}
		arg = std::move(stack_.top());
		stack_.pop();
		return true;
	}
	void WaitPop(T& arg) {
		std::unique_lock<std::mutex> lock(mutex_);

		// 이 조건함수는 notify_one이나 notify_all이 호출될때 호출된다.
		// 그때까지 스레드는 대기하고, 조건함수가 true이면 깨어난다.
		cond_.wait(lock, [this]() {return stack_.empty() == false});
		arg = std::move(stack_.top());
		stack_.pop();
	}
	

private:
	std::stack<T> stack_;
	std::mutex mutex_;
	std::condition_variable cond_;
};

