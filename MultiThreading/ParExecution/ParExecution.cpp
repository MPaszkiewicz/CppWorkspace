
#include <algorithm>
#include <vector>
#include <set>
#include <iostream>
#include <execution>

void runParExecution()
{
    std::vector<uint64_t> vals(25000000);

    std::for_each(std::execution::par, vals.begin(), vals.end(), [](uint64_t& val)
        {
            val = std::hash<std::thread::id>{}(std::this_thread::get_id());
        });

    std::set<uint64_t> thread_id_count{vals.begin(), vals.end()};

    std::cout << thread_id_count.size() << std::endl;
}

int main()
{
    runParExecution();
    return 0;
}