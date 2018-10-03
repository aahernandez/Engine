#pragma once

#include "CriticalSection.hpp"
#include <queue>

template <typename T>
class ThreadSafeQueue
{
public:
	~ThreadSafeQueue()
	{
		int a = 10;
		a = a;
	}

	void Push(T const &v)
	{
		SCOPE_LOCK(m_lock);
		m_queue.push(v);
	}

	bool Empty()
	{
		SCOPE_LOCK(m_lock);
		bool result = m_queue.empty();

		return result;
	}

	bool Pop(T *out)
	{
		SCOPE_LOCK(m_lock);

		if (m_queue.empty()) {
			return false;
		}
		else {
			*out = m_queue.front();
			m_queue.pop();
			return true;
		}
	}

	T Front()
	{
		SCOPE_LOCK(m_lock);
		T result = m_queue.front();

		return result;
	}

public:
	std::queue<T> m_queue;
	CriticalSection m_lock;
};