#pragma once

#include <functional>
#include <fstream>
#include <filesystem>
#include <thread>
#include <iostream>

#include <chrono>

using namespace std::chrono_literals;

void async_read_file(const std::filesystem::path& path, char* buffer, size_t size, std::function<void()> callback)
{
	std::jthread([path, buffer, size, cb = std::move(callback)]()
	{
		std::cout << "starting to read file on thread: " << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(2s);
		std::ifstream input{ path };
		if (input)
		{
			input.read(buffer, size);
		}
		std::cout << "finished to read file on thread: " << std::this_thread::get_id() << std::endl;
		cb();

	}).detach();
}

void async_write_file(const std::filesystem::path& path, char* buffer, size_t size, std::function<void()> callback)
{
	std::jthread([path, buffer, size, cb = std::move(callback)]()
	{
		std::cout << "starting to write file on thread: " << std::this_thread::get_id() << std::endl;
		std::this_thread::sleep_for(2s);
		std::ofstream output{ path };
		output.write(buffer, size);
		std::cout << "finished to write file on thread: " << std::this_thread::get_id() << std::endl;
		cb();

	}).detach();
}