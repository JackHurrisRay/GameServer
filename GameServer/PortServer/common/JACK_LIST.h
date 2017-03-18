#ifndef _JACK_LIST_H
#define _JACK_LIST_H

#include "./../core/MemAllocPool.h"

template<class T>
class JACK_LIST
{
public:

	//////////////////////////////////////////////////////////////////////////
	struct JACK_LIST_CELL
	{
		T*               _data;

		JACK_LIST_CELL* _next;
		JACK_LIST_CELL* _prev;

		void initData(T* t)
		{
			_data = t;
			_next = NULL;
			_prev = NULL;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	typedef MemAllocPool<JACK_LIST_CELL> CELL_ALLOC;
	

protected:
	CELL_ALLOC      _CELL_ALLOC;

	JACK_LIST_CELL* m_start;
	JACK_LIST_CELL* m_end;
	unsigned int    m_count;

public:

	JACK_LIST(void):
		m_start(0),
		m_end(0),
		m_count(0),
		_CELL_ALLOC(CELL_ALLOC(16))
	{

	}

	~JACK_LIST(void)
	{

	}

	void push_back(T* t)
	{
		JACK_LIST_CELL* _cell = _CELL_ALLOC.createData();
		_cell->initData(t);

		if( m_start == NULL && m_end == NULL )
		{
			m_start = _cell;
			m_end   = _cell;

			m_count = 1;
		}
		else
		{
			m_end->_next = _cell;
			_cell->_prev = m_end;

			m_end = _cell;
			m_count += 1;
		}

	}

	//Á´±íÊ×Î²±Õ»·
	void close_loop()
	{
		if( m_count > 1 )
		{
			m_start->_prev = m_end;
			m_end->_next = m_start;
		}
	}

	void clear()
	{
		//////////////////////////////////////////////////////////////////////////
		m_start->_prev = NULL;
		m_end->_next   = NULL;

		//////////////////////////////////////////////////////////////////////////
		std::list<JACK_LIST_CELL*> _DEL_LIST;

		for( JACK_LIST_CELL* _cell = m_start; _cell != NULL; _cell = _cell->_next )
		{
			_DEL_LIST.push_back(_cell);
		}

		m_start = 0;
		m_end   = 0;
		m_count = 0;

		while(_DEL_LIST.size()>0)
		{
			JACK_LIST_CELL* _cell = *_DEL_LIST.begin();
			_DEL_LIST.pop_front();

			_CELL_ALLOC.releaseData(_cell);
		}
	}
};

#endif