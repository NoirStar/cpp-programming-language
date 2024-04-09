#include "pch.h"
#include "ThreadManager2.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"

ThreadManager2::ThreadManager2() {
	// MainThread 1
	InitTLS();
}

ThreadManager2::~ThreadManager2() {
	Join();
}


void ThreadManager2::Launch(std::function<void(void)> callback) {
	LockGuard lock(mutex_);
	// = 은 callback을 호출하기위해
	threads_.push_back(thread([=]() {
		InitTLS();
		callback();
		DestroyTLS();
	}));
}

void ThreadManager2::Join() {
	for (thread& t : threads_) {
		if (t.joinable()) {
			t.join();
		}
	}
	threads_.clear();
}

void ThreadManager2::InitTLS() {
	static std::atomic<int> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager2::DestroyTLS() {
	
}