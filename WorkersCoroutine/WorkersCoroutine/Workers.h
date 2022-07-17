#pragma once

#include "AsyncIO_Stub.h"

#include <deque>
#include <mutex>
#include <thread>
#include <vector>
#include <condition_variable>
#include <iostream>
#include <coroutine>
#include <filesystem>

template<typename WorkItemHandle>
class WorkersManager
{
public:
	WorkersManager()
	{
		for (size_t i = 0; i < workers_count; ++i)
		{
			m_workers.emplace_back([this](std::stop_token stop_token) { this->worker_func(stop_token); });
		}
	}

	~WorkersManager()
	{
		for (auto& worker : m_workers)
		{
			worker.request_stop();
		}
		m_condition.notify_all();
	}

	void add_work_task(WorkItemHandle item)
	{
		{
			std::scoped_lock lock(m_queue_mux);
			m_work_queue.push_back(item);
		}
		m_condition.notify_one();
	}

	class TaskBase
	{
	public:
		TaskBase(WorkersManager<WorkItemHandle>& workers_manager)
			: m_workers_manager(workers_manager)
		{
		}

		bool await_ready()
		{
			return false;
		}

		void await_resume()
		{
			std::cout << "Resuming Task on thread: " << std::this_thread::get_id() << std::endl;
		}

		void add_task(WorkItemHandle item)
		{
			m_workers_manager.add_work_task(item);
		}

		WorkersManager<WorkItemHandle>& m_workers_manager;
	};

	class TaskWrite : public TaskBase
	{
	public:
		TaskWrite(WorkersManager<WorkItemHandle>& workers_manager, std::filesystem::path filePath, char* buffer, size_t size)
			: TaskBase(workers_manager)
			, m_filePath(std::move(filePath))
			, m_buffer(buffer)
			, m_size(size)
		{
		}

		void await_suspend(WorkItemHandle handle)
		{
			std::cout << "TaskWrite::await_suspend on thread: " << std::this_thread::get_id() << std::endl;
			async_write_file(m_filePath, m_buffer, m_size,
							 [this, handle]()
							 {
								this->add_task(handle);
							 });
		}

		std::filesystem::path m_filePath;
		char* m_buffer;
		size_t m_size;
	};

	class TaskRead : public TaskBase
	{
	public:
		TaskRead(WorkersManager<WorkItemHandle>& workers_manager, std::filesystem::path filePath, char* buffer, size_t size)
			: TaskBase(workers_manager)
			, m_filePath(std::move(filePath))
			, m_buffer(buffer)
			, m_size(size)
		{
		}

		void await_suspend(WorkItemHandle handle)
		{
			std::cout << "TaskWrite::await_suspend on thread: " << std::this_thread::get_id() << std::endl;
			async_read_file(m_filePath, m_buffer, m_size,
							[this, handle]()
							{
								this->add_task(handle);
							});
		}

		std::filesystem::path m_filePath;
		char* m_buffer;
		size_t m_size;
	};

	TaskWrite createWriteTask(const std::filesystem::path& filePath, char* buffer, size_t size)
	{
		return TaskWrite{ *this, filePath, buffer, size };
	}

	TaskRead createReadTask(const std::filesystem::path& filePath, char* buffer, size_t size)
	{
		return TaskRead{ *this, filePath, buffer, size };
	}

private:

	void worker_func(std::stop_token stop_token)
	{
		while (!stop_token.stop_requested())
		{
			std::unique_lock lock(m_queue_mux);
			std::cout << "worker: " << std::this_thread::get_id() << " waiting for work.\n";
			m_condition.wait(lock, [this, &stop_token]() { return m_work_queue.size() > 0 ||
				stop_token.stop_requested(); });
			if (stop_token.stop_requested())
			{
				std::cout << "worker: " << std::this_thread::get_id() << " received stop request.\n";
				return;
			}
			std::cout << "worker: " << std::this_thread::get_id() << " received work item.\n";
			auto work_task = m_work_queue.front();
			m_work_queue.pop_front();
			lock.unlock();
			work_task.resume();
		}
	}

private:
	std::mutex m_queue_mux;
	std::condition_variable_any m_condition;
	constexpr static size_t workers_count = 3;
	std::vector<std::jthread> m_workers;
	std::deque<WorkItemHandle> m_work_queue;
};