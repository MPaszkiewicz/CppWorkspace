#include "AsyncIO_Stub.h"
#include "Workers.h"

#include <coroutine>
#include <iostream>
#include <future>
#include <array>

class TaskPromise;

class TaskCoroutine
{
public:
	using promise_type = TaskPromise;
	using handle_type = std::coroutine_handle<promise_type>;

	TaskCoroutine(handle_type promise);

	~TaskCoroutine()
	{
		std::cout << "~TaskCoroutine called!\n";
		if (m_promise_handle)
		{
			std::cout << "destroying m_promise_handle!\n";
			m_promise_handle.destroy();
		}
	}

	handle_type getHandle()
	{
		return m_promise_handle;
	}

	void getResult();

	void resume()
	{
		m_promise_handle.resume();
	}

private:
	handle_type m_promise_handle;
	std::future<void> m_taskFinish;
};

class TaskPromise
{
public:
	~TaskPromise()
	{
		std::cout << "~TaskPromise called!\n";
	}

	TaskCoroutine get_return_object()
	{
		return TaskCoroutine{ TaskCoroutine::handle_type::from_promise(*this) };
	}

	std::suspend_always initial_suspend() noexcept
	{
		return {};
	}

	std::suspend_always final_suspend() noexcept
	{
		std::cout << "final_suspend called" << std::endl;
		m_promise.set_value();
		return {};
	}

	void unhandled_exception()
	{
		std::cout << "unhandled exception called!\n";
	}

	void return_value(std::tuple<char*, size_t> returnVal)
	{
		std::cout << "Task returned" << std::endl;
		m_return_buffer = std::get<0>(returnVal);
		m_return_size = std::get<1>(returnVal);
	}

	std::future<void> getFuture()
	{
		return m_promise.get_future();
	}

	std::promise<void> m_promise{};
	char* m_return_buffer;
	size_t m_return_size;
};

TaskCoroutine::TaskCoroutine(handle_type promise_handle)
	: m_promise_handle(promise_handle)
{
	m_taskFinish = m_promise_handle.promise().getFuture();
}

void TaskCoroutine::getResult()
{
	m_taskFinish.get();
}

void processData(char* buffer, size_t size, char delta)
{
	for (size_t i = 0; i < size; ++i)
	{
		buffer[i] += delta;
	}
}

TaskCoroutine coroTask(WorkersManager<TaskCoroutine::handle_type>& workers_manager, const char* input, const char* output, char delta)
{
	static constexpr size_t BUFFER_SIZE = 32;
	std::array<char, BUFFER_SIZE> m_buffer{};

	std::cout << "Starting coroTask on thread: " << std::this_thread::get_id() << std::endl;
	co_await workers_manager.createReadTask(input, m_buffer.data(), m_buffer.size());
	processData(m_buffer.data(), m_buffer.size(), delta);
	co_await workers_manager.createWriteTask(output, m_buffer.data(), m_buffer.size());
	std::cout << "Finished coroTask on thread: " << std::this_thread::get_id() << std::endl;
	co_return std::tuple(m_buffer.data(), m_buffer.size());
}

void printData(char* buffer, size_t size)
{
	for (size_t i = 0; i < size; ++i)
	{
		std::cout << buffer[i];
	}
	std::cout << std::endl;
}

int main()
{
	{
		auto manager = WorkersManager<TaskCoroutine::handle_type>{};
		std::this_thread::sleep_for(2s);
		{
			auto task1 = coroTask(manager, "input.txt", "output1", 1);
			auto task2 = coroTask(manager, "input.txt", "output2", 2);
			auto task3 = coroTask(manager, "input.txt", "output3", 3);

			manager.add_work_task(task1.getHandle());
			manager.add_work_task(task2.getHandle());
			manager.add_work_task(task3.getHandle());

			task1.getResult();
			task2.getResult();
			task3.getResult();

			printData(task1.getHandle().promise().m_return_buffer, task1.getHandle().promise().m_return_size);
			printData(task2.getHandle().promise().m_return_buffer, task2.getHandle().promise().m_return_size);
			printData(task3.getHandle().promise().m_return_buffer, task3.getHandle().promise().m_return_size);
		}
	}
	std::cout << "main finish\n";
}