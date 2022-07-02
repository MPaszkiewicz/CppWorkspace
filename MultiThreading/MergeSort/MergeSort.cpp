
#include <iostream>
#include <vector>
#include <algorithm>
#include <future>

template <typename T, typename Cmp = std::less<T>>
void mergeSort(std::vector<T>& data, size_t begin, size_t end, Cmp comparator = Cmp{})
{
    auto size = end - begin;
    if (size < 2)
    {
        return;
    }
    if (size == 2)
    {
        if (!comparator(data[begin], data[begin + 1]))
        {
            std::swap(data[begin], data[begin + 1]);
        }
        return;
    }

    auto mid = size / 2;
    std::vector<T> workspace(size);
    std::copy(data.begin() + begin, data.begin() + begin + size, workspace.begin());
    mergeSort<T, Cmp>(workspace, 0, mid);
    mergeSort<T, Cmp>(workspace, mid, size);

    auto lidx = 0;
    auto ridx = mid;
    auto pos = begin;
    while (lidx < mid && ridx < size)
    {
        if (comparator(workspace[lidx], workspace[ridx]))
        {
            data[pos++] = workspace[lidx++];
        }
        else
        {
            data[pos++] = workspace[ridx++];
        }
    }
    while (lidx < mid)
    {
        data[pos++] = workspace[lidx++];
    }
    while (ridx < size)
    {
        data[pos++] = workspace[ridx++];
    }
}


template <typename T, typename Cmp = std::less<T>>
void mergeSortMultiThread(std::vector<T>& data, size_t begin, size_t end, Cmp comparator = Cmp{})
{
    auto size = end - begin;
    if (size < 2)
    {
        return;
    }
    if (size == 2)
    {
        if (!comparator(data[begin], data[begin + 1]))
        {
            std::swap(data[begin], data[begin + 1]);
        }
        return;
    }

    auto mid = size / 2;
    std::vector<T> workspace(size);
    std::copy(data.begin() + begin, data.begin() + begin + size, workspace.begin());
    auto future = std::async(std::launch::async, [&]() { mergeSort<T, Cmp>(workspace, 0, mid); });
    mergeSort<T, Cmp>(workspace, mid, size);
    future.get();

    auto lidx = 0;
    auto ridx = mid;
    auto pos = begin;
    while (lidx < mid && ridx < size)
    {
        if (comparator(workspace[lidx], workspace[ridx]))
        {
            data[pos++] = workspace[lidx++];
        }
        else
        {
            data[pos++] = workspace[ridx++];
        }
    }
    while (lidx < mid)
    {
        data[pos++] = workspace[lidx++];
    }
    while (ridx < size)
    {
        data[pos++] = workspace[ridx++];
    }
}

int main()
{
    std::vector<int> data = {10, 51, 4, 2, 1, 63, 100, 52, 11, 0};

    mergeSortMultiThread(data, 0, data.size());

    for (int v : data)
    {
        std::cout << v << " ";
    }
    std::cout << std::endl;

    return 0;
}
