#include <numeric>
#include <algorithm>
#include <cstdint>
#include <string>
#include <mutex>
#include <iostream>
#include <map>
#include <array>
#include <random>
#include <chrono>

using namespace std::chrono_literals;

class Bank
{
public:
    Bank() = default;

    void addAccount(std::string name, int64_t amount)
    {
        m_accounts[name] = amount;
    }

    void transfer(const std::string& from, const std::string& to, int64_t amount)
    {
        std::scoped_lock lock(m_mutex);
        m_accounts[from] -= amount;
        std::this_thread::yield();
        m_accounts[to] += amount;
    }

    int64_t sum()
    {
        std::scoped_lock lock(m_mutex);
        return std::accumulate(m_accounts.begin(), m_accounts.end(), uint64_t{}, [](int64_t cur, auto& it) { return cur + it.second; });
    }


private:
    std::mutex m_mutex;
    std::map<std::string, int64_t> m_accounts;
};

std::mutex ioMutex;

void printSum(Bank& b)
{
    std::scoped_lock lock(ioMutex);
    std::cout << "current sum: " << b.sum() << std::endl;
}

constexpr int SIZEN = 10;
std::array<const char*, SIZEN> names = { "Mat", "George", "Chris", "Art", "Mario", "Karen", "Jessie", "Zoe", "Buzz", "Zurg" };

void runBank()
{
    std::random_device rd{};
    std::mt19937 gen{ rd() };
    std::normal_distribution<> amountDist{ 300, 150 };
    std::uniform_int_distribution accIdxDist{ 0, SIZEN - 1 };

    Bank b;
    for (const auto& name : names)
    {
        b.addAccount(name, amountDist(gen));
    }
    printSum(b);

    std::jthread printThread([&](std::stop_token token)
        {
            while (!token.stop_requested())
            {
                printSum(b);
                std::this_thread::sleep_for(200ms);
            }
        });

    std::vector<std::jthread> transferThreads(8);
    for (auto& t : transferThreads)
    {
        t = std::jthread([&]()
            {
                for (auto i = 0; i < 150; ++i)
                {
                    int accIdx1 = accIdxDist(gen);
                    int accIdx2 = accIdxDist(gen);
                    if (accIdx1 == accIdx2) accIdx2 = (accIdx2 + 1) % SIZEN;
                    b.transfer(names[accIdx1], names[accIdx2], amountDist(gen));
                    std::this_thread::sleep_for(50ms);
                }
            });
    }
}

int main()
{
    runBank();
    return 0;
}