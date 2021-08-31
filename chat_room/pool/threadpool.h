#pragma once
#include<condition_variable>
#include<mutex>
#include<queue>
#include<thread>
#include<iostream>
using namespace std;
class threadpool
{

public:
	threadpool(size_t maxnum=8):pool(make_shared<TaskPool>()) {
		std::cout << "tp create" << endl;
		for (int i = 0; i < maxnum; i++) {
			thread([pool = pool] {

				unique_lock<mutex>locker(pool->mtx);

				while (true) {
					if (!pool->tasks.empty()) {
						auto task = pool->tasks.front();
						locker.unlock();
						task();
						locker.lock();
					}
					else if (pool->isClosed)	break;
					else {
						pool->cond.wait(locker);
					}
				}
				}
			).detach();
		}
	}



	threadpool() = default;
	~threadpool() {
		if (static_cast<bool>(pool)) {
			std::lock_guard<std::mutex> locker(pool->mtx);
			pool->isClosed = true;
			pool->cond.notify_all();
		}
	
	}
	template<class F>
	void AddTask(F&& task) {
		{
			std::lock_guard<std::mutex> locker(pool->mtx);
			pool->tasks.emplace(std::forward<F>(task));
		}
		pool->cond.notify_one();    //给线程池唤醒了  只唤醒第一个线程 不存在争用
	}
private:
	struct TaskPool
	{
		condition_variable cond;
		queue<function<void()>>tasks;
		bool isClosed;
		mutex mtx;
	};
	size_t maxnum;

	std::shared_ptr<TaskPool> pool;
};

