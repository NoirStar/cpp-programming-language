#pragma once

#include <thread>
#include <functional>

class ThreadManager2 {
public:
	ThreadManager2();
	~ThreadManager2();

	void Launch(std::function<void(void)> callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();
	
private:
	Mutex mutex_;
	vector<thread> threads_;
};

