// ReadWrite.cpp : Defines the entry point for the application.
//

#include <iostream>
#include <algorithm>
#include <barrier>
#include <chrono>
#include <mutex>
#include <vector>
#include <shared_mutex>

using namespace std::chrono_literals;

void readWriteMutex()
{
    std::shared_mutex sharedMux;
    std::mutex ioMutex;
    std::string myString = "        ";
    bool writerWaiting = false;
    std::condition_variable_any cond;
    std::jthread writer = std::jthread([&](std::stop_token token)
    {
        while (!token.stop_requested())
        {
            {
                writerWaiting = true;
                std::scoped_lock lock(sharedMux);
                std::for_each(myString.begin(), myString.end(), [](char& val) { val = 'x'; });
                writerWaiting = false;
            }
            cond.notify_all();
            std::this_thread::sleep_for(50ms);
            {
                writerWaiting = true;
                std::scoped_lock lock(sharedMux);
                std::for_each(myString.begin(), myString.end(), [](char& val) { val = ' '; });
                writerWaiting = false;
            }
            cond.notify_all();
            std::this_thread::sleep_for(50ms);
        }
    });

    {
        std::vector<std::jthread> readers(3);
        for (auto& reader : readers)
        {
            reader = std::jthread([&]()
                {
                    for (int i = 0; i < 50; ++i)
                    {
                        std::string strCopy;
                        {
                            std::shared_lock lock(sharedMux);
                            cond.wait(lock, [&]() { return !writerWaiting; });
                            strCopy = myString;
                        }
                        {
                            std::scoped_lock lock(ioMutex);
                            std::cout << "read: <" << strCopy << ">\n";
                        }
                        std::this_thread::sleep_for(50ms);
                    }
                });
        }
    }
}

void readWriteBarrier()
{
    std::mutex ioMutex;
    std::string myString = "        ";
    std::barrier barrier(3, []() noexcept
    {
        std::cout << "------------------ phase completed ------------------\n";
    });

    std::vector<std::jthread> writers(3);
    std::mutex writerMux;
    char curChar = 'a';
    for (auto& writer : writers)
    {
        writer = std::jthread([&, myChar = curChar++](std::stop_token token)
        {
            while (!token.stop_requested())
            {
                {
                    std::scoped_lock lock(writerMux);
                    std::for_each(myString.begin(), myString.end(), [&](char& val)
                        { val = myChar; std::this_thread::sleep_for(20ms); });
                }
                barrier.arrive_and_wait();
            }
        });
    }


    {
        std::vector<std::jthread> readers(3);
        for (auto& reader : readers)
        {
            reader = std::jthread([&]()
                {
                    for (int i = 0; i < 500; ++i)
                    {
                        std::string strCopy;
                        {
                            strCopy = myString;
                        }
                        {
                            std::scoped_lock lock(ioMutex);
                            std::cout << "read: <" << strCopy << ">\n";
                        }
                        std::this_thread::sleep_for(10ms);
                    }
                });
        }
    }
}


int main()
{
    //readWriteMutex();
    readWriteBarrier();
    return 0;
}
