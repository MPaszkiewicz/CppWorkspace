#pragma once

template<typename T>
class vector
{
public:
	vector(int capacity = 2)
		: m_data(nullptr)
		, m_size(0)
		, m_capacity(capacity)
	{
		if (capacity > 0)
		{
			m_data = new T[capacity];
		}
	}

	~vector()
	{
		delete[] m_data;
	}

	void resize(int capacity)
	{
		if (capacity <= m_capacity)
		{
			return;
		}
		T* data = new T[capacity];
		for (int i = 0; i < m_size; ++i)
		{
			data[i] = m_data[i];
		}
		delete[] m_data;
		m_data = data;
		m_capacity = capacity;
	}

	void push_back(const T& val)
	{
		if (m_size == m_capacity)
		{
			resize(m_capacity * 2);
		}
		m_data[m_size++] = val;
	}

	void pop_back()
	{
		--m_size;
	}

	const T& back()
	{
		return m_data[m_size - 1];
	}

	int size()
	{
		return m_size;
	}

	T& operator[](int i)
	{
		return m_data[i];
	}

	T* data()
	{
		return m_data;
	}

private:
	T* m_data;
	int m_size;
	int m_capacity;
};