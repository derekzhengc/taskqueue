#pragma once
#include <thread>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <memory>
#include <vector>
#include "AsyncResult.h"
class AsyncBase
{
public:
	using Task = std::function<void()>;
	struct Node
	{
		int id;
		Task task;
	};

	AsyncBase(unsigned int threadCount){
		for (unsigned int i = 0; i < threadCount; ++i)
		{
			auto _thread = std::shared_ptr<std::thread>(new std::thread([this]() {
				for (;;)
				{
					std::function<void()> task;
					{
						std::unique_lock<std::mutex> lock(this->queue_mutex);
						this->condition.wait(lock,
							[this] { return this->stop || !this->tasks.empty(); });
						if (this->stop && this->tasks.empty())
							return;
						task = std::move(this->tasks.begin())->second;
						this->tasks.erase(this->tasks.begin());
					}

					task();
				}
				}));

			_thread_list.push_back(_thread);

		}
	}

	~AsyncBase()
	{
		stopTask();
	}

	void stopTask()
	{
		stop = true;
		condition.notify_all();
		for (auto th : _thread_list)
		{
			if (th->joinable())
			{
				th->join();
			}
		}
	}


	template<typename F, typename... Args>
	auto emplace(F&& f, Args&&... args) ->AsyncResult<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;

		auto task = std::make_shared<std::packaged_task<return_type()> >(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);
		;
		std::future<return_type> res = task->get_future();
		{
			std::unique_lock<std::mutex> lock(queue_mutex);

			if (stop)
				throw std::runtime_error("enqueue on stopped taskqueue");

			int id = insert([task]() { (*task)(); });
			
			auto func = [id, this]() {
				std::lock_guard<std::mutex> lock(queue_mutex);
				tasks.erase(id); };
			condition.notify_one();
			return AsyncResult<return_type>(std::move(func), std::move(res));
		}
		
	}




	AsyncBase(const AsyncBase&) = delete;
	AsyncBase& operator=(const AsyncBase&) = delete;
private:
	int insert(Task&& task)
	{
		++last_id;
		tasks.insert({ last_id, task });

		return last_id;
	}
	
private:
	
	std::vector<std::shared_ptr<std::thread> > _thread_list;
	std::map< int, Task > tasks;  //容器必须支持能拷贝或者移动构造的类型
	//std::queue< std::function<void()> > tasks;
	std::mutex queue_mutex;
	std::condition_variable condition;
	std::atomic_bool stop{ false };
	std::atomic_int last_id{ 0 };
};