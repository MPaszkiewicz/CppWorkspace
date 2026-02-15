#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <cstdlib>

#include "merge_sort.h" // expects to be in the same directory
#include "quick_sort.h"

template<typename T>
bool equal_sorted(const std::vector<T>& a)
{
    std::vector<T> expected = a;
    std::sort(expected.begin(), expected.end());
    return a == expected;
}

template<typename T, typename Sort>
bool run_case(std::vector<T> input, const char* case_name, Sort sort, const char* sort_name)
{
    std::vector<T> arr = input;
    // avoid UB for empty ranges: call with a valid pointer when empty
    if (arr.empty())
    {
        T tmp{};
        sort(&tmp, &tmp);
        std::cout << "[ OK ] " << sort_name << " : " << case_name << " (empty)\n";
        return true;
    }

    sort(arr.data(), arr.data() + arr.size());

    if (!equal_sorted(arr))
    {
        std::cerr << "[FAIL] " << sort_name << " : " << case_name << "\n";
        std::cerr << "  result:   ";
        for (auto& v : arr) std::cerr << v << " ";
        std::cerr << "\n  expected: ";
        std::vector<T> expected = input;
        std::sort(expected.begin(), expected.end());
        for (auto& v : expected) std::cerr << v << " ";
        std::cerr << "\n";
        return false;
    }

    std::cout << "[ OK ] " << sort_name << " : " << case_name << "\n";
    return true;
}

struct Counted
{
    int v;
    static int copies;
    static int moves;
    static int assigns;

    Counted() : v(0) {}
    Counted(int x) : v(x) {}
    Counted(const Counted& o) : v(o.v) { ++copies; }
    Counted(Counted&& o) noexcept : v(o.v) { ++moves; }
    Counted& operator=(const Counted& o) { v = o.v; ++assigns; return *this; }
    bool operator<(const Counted& o) const { return v < o.v; }
    bool operator==(const Counted& o) const { return v == o.v; }

    friend std::ostream& operator<<(std::ostream& out, const Counted& c)
    {
        return out << c.v;
    }
};

int Counted::copies = 0;
int Counted::moves = 0;
int Counted::assigns = 0;

// Sort must be callable as void(T* begin, T* end).
template<typename Sort>
bool run_all_tests(const char* sort_name, Sort sort)
{
    bool ok = true;

    // simple integer cases
    ok &= run_case<int>({}, "empty ints", sort, sort_name);
    ok &= run_case<int>({42}, "single int", sort, sort_name);
    ok &= run_case<int>({1,2,3,4,5}, "already sorted ints", sort, sort_name);
    ok &= run_case<int>({5,4,3,2,1}, "reversed ints", sort, sort_name);
    ok &= run_case<int>({3,1,4,1,5,9,2,6,5,3,5}, "ints with duplicates", sort, sort_name);

    // random ints (deterministic seed so different sort implementations exercise same data)
    {
        std::mt19937_64 rng(123);
        std::uniform_int_distribution<int> dist(-1000, 1000);
        std::vector<int> v(1000);
        for (auto& x : v) x = dist(rng);
        ok &= run_case<int>(v, "random ints (1000)", sort, sort_name);
    }

    // strings
    ok &= run_case<std::string>({"z", "a", "abc", "ab", "a"}, "std::string test", sort, sort_name);

    // Counted type test (small) - reset counters between runs and print statistics
    {
        std::vector<Counted> vc;
        for (int i = 10; i >= 0; --i) vc.emplace_back(i);

        Counted::copies = Counted::moves = Counted::assigns = 0;
        bool r = run_case<Counted>(vc, "Counted type (move/copy tracking)", sort, sort_name);
        std::cout << "  " << sort_name << ": copies=" << Counted::copies << " moves=" << Counted::moves << " assigns=" << Counted::assigns << "\n";
        ok &= r;
    }

    return ok;
}

int main()
{
    bool ok = true;

    // Run tests for merge_sort
    ok &= run_all_tests("merge_sort", [](auto* b, auto* e){ merge_sort(b, e); });

    // Run tests for quick_sort
    ok &= run_all_tests("quick_sort", [](auto* b, auto* e){ quick_sort(b, e); });

    std::cout << (ok ? "ALL TESTS PASSED\n" : "SOME TESTS FAILED\n");
    return ok ? 0 : 1;
}