
#include <memory>
#include <vector>
#include <future>
#include <mutex>
#include <optional>
#include <iostream>

using namespace std::chrono_literals;

int computationWorker(std::shared_ptr<std::mutex> mux, std::vector<int>& dataQueue, std::shared_ptr<int> currentIdx) noexcept
{
    int localResult = 0;
    while (true)
    {
        std::optional<int> idx;
        {
            std::scoped_lock lock(*mux);
            if (*currentIdx < dataQueue.size())
            {
                idx = (*currentIdx)++;
            }
        }
        if (idx)
        {
            localResult += dataQueue[*idx] * dataQueue[*idx];
            std::this_thread::sleep_for(150ms);
        }
        else
        {
            return localResult;
        }
    }
}

//void completionWorker(std::vector<std::future<int>> workerResults, std::promise<int> resultProm)
//{
//    int totalResult = 0;
//    for (auto& results : workerResults)
//    {
//        totalResult += results.get();
//    }
//    resultProm.set_value(totalResult);
//}

int completionWorker(std::vector<std::future<int>> workerResults)
{
    int totalResult = 0;
    for (auto& results : workerResults)
    {
        totalResult += results.get();
    }
    return totalResult;
}

std::future<int> runComputation(std::vector<int>& inputData, int workerCount)
{
    std::vector<std::future<int>> workerResults(workerCount);
    auto indexMux = std::make_shared<std::mutex>();
    auto currentIdx = std::make_shared<int>(0);
    for (auto& worker : workerResults)
    {
        worker = std::async(std::launch::async, [currentIdx, indexMux, &inputData]() { return computationWorker(indexMux, inputData, currentIdx); });
    }
    //auto resultProm = std::promise<int>{};
    //auto resultFuture = resultProm.get_future();

    //std::jthread{ completionWorker, std::move(workerResults), std::move(resultProm) }.detach();
    //return resultFuture;

    return std::async(std::launch::async, completionWorker, std::move(workerResults));
}

int main()
{

    std::vector<int> data{ 2, 3, 4};
    auto computeFuture = runComputation(data, 2);

    std::cout << computeFuture.get() << std::endl;

    return 0;
}