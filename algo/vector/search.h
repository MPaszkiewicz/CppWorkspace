#pragma once


template<typename T>
int binary_search(T* data, int begin, int end, const T& value)
{
	while (begin < end)
	{
		int mid = begin + (end - begin) / 2;
		if (data[mid] < value)
		{
			begin = mid + 1;
		}
		else if (data[mid] > value)
		{
			end = mid;
		}
		else
		{
			return mid;
		}
	}
	return -1;
}

template<typename T>
int lower_bound(T* data, int begin, int end, const T& value)
{
	while (begin < end)
	{
		int mid = begin + (end - begin) / 2;
		if (data[mid] < value)
		{
			begin = mid + 1;
		}
		else
		{
			end = mid;
		}
	}
	return end;
}

template<typename T>
int upper_bound(T* data, int begin, int end, const T& value)
{
	while (begin < end)
	{
		int mid = begin + (end - begin) / 2;
		if (data[mid] <= value)
		{
			begin = mid + 1;
		}
		else
		{
			end = mid;
		}
	}
	return end;
}