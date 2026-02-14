#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <cstdlib>

#include "merge_sort.h" // expects to be in the same directory

template<typename T>
bool equal_sorted(const std::vector<T>& a)
{
    std::vector<T> expected = a;
    std::sort(expected.begin(), expected.end());
    return a == expected;
}

template<typename T>
bool run_case(std::vector<T> input, const char* name)
{
    std::vector<T> arr = input;
    // avoid UB for empty ranges: call with a valid pointer when empty
    if (arr.empty())
    {
        T tmp{};
        merge_sort(&tmp, &tmp);
        std::cout << "[ OK ] " << name << " (empty)\n";
        return true;
    }

    merge_sort(arr.data(), arr.data() + arr.size());

    if (!equal_sorted(arr))
    {
        std::cerr << "[FAIL] " << name << "\n";
        std::cerr << "  result:   ";
        for (auto& v : arr) std::cerr << v << " ";
        std::cerr << "\n  expected: ";
        std::vector<T> expected = input;
        std::sort(expected.begin(), expected.end());
        for (auto& v : expected) std::cerr << v << " ";
        std::cerr << "\n";
        return false;
    }

    std::cout << "[ OK ] " << name << "\n";
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

int main()
{
    bool ok = true;

    ok &= run_case<int>({}, "empty ints");
    ok &= run_case<int>({42}, "single int");
    ok &= run_case<int>({1,2,3,4,5}, "already sorted ints");
    ok &= run_case<int>({5,4,3,2,1}, "reversed ints");
    ok &= run_case<int>({3,1,4,1,5,9,2,6,5,3,5}, "ints with duplicates");

    // random ints
    {
        std::mt19937_64 rng(123);
        std::uniform_int_distribution<int> dist(-1000, 1000);
        std::vector<int> v(1000);
        for (auto& x : v) x = dist(rng);
        ok &= run_case<int>(v, "random ints (1000)");
    }

    // std::string test
    ok &= run_case<std::string>({"z", "a", "abc", "ab", "a"}, "std::string test");

    // Counted type test (small)
    {
        Counted::copies = Counted::moves = Counted::assigns = 0;
        std::vector<Counted> vc;
        for (int i = 10; i >= 0; --i) vc.emplace_back(i);
        // use run_case with moveable type: copy/move counters will update
        bool r = run_case<Counted>(vc, "Counted type (move/copy tracking)");
        std::cout << "  copies=" << Counted::copies << " moves=" << Counted::moves << " assigns=" << Counted::assigns << "\n";
        ok &= r;
    }

    std::cout << (ok ? "ALL TESTS PASSED\n" : "SOME TESTS FAILED\n");
    return ok ? 0 : 1;
}