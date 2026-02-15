#pragma once

#include <cstddef>

template<typename T>
T* get_median(T* a, T* b, T* c)
{
	if (*a < *b)
	{
		if (*b < *c) return b; // a < b < c
		if (*a < *c) return c; // a < c <= b
		return a;			   // c <= a < b
	}
	else
	{
		if (*a < *c) return a; // b <= a < c
		if (*b < *c) return c; // b < c <= a
		return b;              // c <= b <= a
	}
}

template<typename T>
T* find_pivot(T* begin, T* end)
{
	T* mid = begin + (end - begin) / 2;
	return get_median(begin, mid, end - 1);
}

template<typename T>
void swap_val(T* a, T* b)
{
	T tmp = *a;
	*a = *b;
	*b = tmp;
}

template<typename T>
T* partition(T* begin, T* end)
{
	T* pivot = find_pivot(begin, end);
	T* last = end - 1;
	swap_val(pivot, last);
	pivot = last;
	T* write_it = begin;
	for (T* scan_it = begin; scan_it != pivot; ++scan_it)
	{
		if (*scan_it < *pivot)
		{
			swap_val(scan_it, write_it);
			++write_it;
		}
	}
	swap_val(pivot, write_it);
	return write_it;
}
template<typename T>
T* partition_hoare(T* begin, T* end)
{

}

template<typename T>
void quick_sort(T* begin, T* end)
{
	std::size_t size = end - begin;
	if (size <= 1)
	{
		return;
	}
	T* pivot = partition(begin, end);
	quick_sort(begin, pivot);
	quick_sort(pivot + 1, end);
}