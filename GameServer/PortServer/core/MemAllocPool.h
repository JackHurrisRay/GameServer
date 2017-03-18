#ifndef _MEMALLOCPOOL_H
#define _MEMALLOCPOOL_H

#include <iostream>
#include <list>
#include <WinSock2.h>
#define BUFFER_SIZE 2048  + 32

struct Locker
{
#ifdef WINDOWS
	CRITICAL_SECTION _mutex;
#endif 

	Locker()
	{
#ifdef WINDOWS
		InitializeCriticalSection(&_mutex);
#endif
	};

	~Locker()
	{
#ifdef WINDOWS
		DeleteCriticalSection(&_mutex);
#endif
	};

	void lock()
	{
#ifdef WINDOWS
		EnterCriticalSection(&_mutex);
#endif
	};

	void unlock()
	{
#ifdef WINDOWS
		LeaveCriticalSection(&_mutex);
#endif
	};
};


template< class T>
class MemAllocPool
{
public:
	typedef std::list<T*> T_LIST;
	const char*           _INFO;

	T_LIST m_usedList;
	T_LIST m_unuseList;
	Locker m_lock;

	MemAllocPool(int _init_count, const char* __INFO = NULL):
		_INFO(__INFO)
	{
		if( _INFO )
		{
			std::cout << _INFO << std::endl;
		}

		m_lock.lock();
		for(int i=0; i<_init_count; i++)
		{
			T* t = new T;
			m_unuseList.push_back(t);
		}
		m_lock.unlock();

		if( _INFO )
		{
			std::cout << "----alloc:" << m_unuseList.size() << "---" << std::endl;
		}
	}

	~MemAllocPool(void)
	{
		if( _INFO )
		{
			std::cout << "----release alloc---" << _INFO << std::endl;
		}

		m_lock.lock();
		while (m_unuseList.size() > 0)
		{
			T* _t = *m_unuseList.begin();
			m_unuseList.pop_front();

			delete _t;
		}

		while (m_usedList.size() > 0)
		{
			T* _t = *m_usedList.begin();
			m_usedList.pop_front();

			delete _t;
		}
		m_lock.unlock();

		if( _INFO )
		{
			std::cout << "----end release alloc---" << std::endl;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	T* createData()
	{
		//////////////////////////////////////////////////////////////////////////
		T* t = NULL;

		m_lock.lock();
		if( m_unuseList.size() > 0 )
		{
			t = *m_unuseList.begin();
			m_unuseList.pop_front();
		}

		if( t == NULL )
		{
			t = new T;
		}

		m_usedList.push_back(t);
		m_lock.unlock();

		return t;
	}

	void releaseData(T* t)
	{
		m_lock.lock();
		if( std::find(m_usedList.begin(), m_usedList.end(), t) != m_usedList.end() )
		{
			m_usedList.remove(t);
			m_unuseList.push_back(t);
		}
		m_lock.unlock();
	}

	void clear()
	{
		m_lock.lock();
		for (T_LIST::iterator _c in m_usedList)
		{
			T* t = *_c;
			m_unuseList.push_back(t);
		}

		m_usedList.clear();
		m_lock.unlock();
	}
};

#endif