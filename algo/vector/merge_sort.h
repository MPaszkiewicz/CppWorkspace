#pragma once

#include "vector.h"

#include <cstddef>
#include <cstring>
#include <type_traits>

template<typename T>
void do_merge_sort(T* begin, T* end, T* ws_begin)
{
	std::size_t size = end - begin;
	if (size <= 1)
	{
		return;
	}
	T* mid = begin + size / 2;
	do_merge_sort(begin, mid, ws_begin);
	do_merge_sort(mid, end, ws_begin + size / 2);
	T* currentL = begin;
	T* currentR = mid;
	T* currentTemp = ws_begin;
	while (currentL < mid && currentR < end)
	{
		if (*currentL < *currentR)
		{
			*currentTemp++ = *currentL++;
		}
		else
		{
			*currentTemp++ = *currentR++;
		}
	}
	while (currentL < mid)
	{
		*currentTemp++ = *currentL++;
	}
	while (currentR < end)
	{
		*currentTemp++ = *currentR++;
	}
	if constexpr (std::is_trivially_copyable_v<T>)
	{
		std::memcpy(begin, ws_begin, size * sizeof(T));
	}
	else
	{
		for (std::size_t i = 0; i < size; ++i)
		{
			begin[i] = ws_begin[i];
		}
	}
}

template<typename T>
void merge_sort(T* begin, T* end)
{
	std::size_t size = end - begin;
	T* temp = new T[size];
	do_merge_sort(begin, end, temp);
	delete[] temp;
}