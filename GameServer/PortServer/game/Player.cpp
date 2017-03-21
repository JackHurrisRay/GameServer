#include "Player.h"
#include "./../core/ClientObject.h"

//////////////////////////////////////////////////////////////////////////
ALLOC_PLAYER Players::_ALLOC_PLAYER = ALLOC_PLAYER(MAX_PLAYER_LIMIT, "PLAYER ALLOC");

//////////////////////////////////////////////////////////////////////////
void BASE_PLAYER::initData()
{
	memset(_KEY, 0, sizeof(_KEY));
	_PLAYER_ID = 0xFFFF;
	_GOLD      = 0;
	_ROOMID    = -1;
	_INDEX     = -1;
	_CLIENT    = NULL;
}

void BASE_OBJECT::release()
{
	BASE_PLAYER* _player = Players::Instance()->get_player(_UID);

	if( _player != NULL )
	{
		_player->_CLIENT = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
class PLAYER_LOCK
{
public:
	PLAYER_LOCK()
	{
		Players::Instance()->m_lock.lock();
	};

	~PLAYER_LOCK()
	{
		Players::Instance()->m_lock.unlock();
	};
};

/************************************************************************/
/*                                                                      */
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
Players::Players(void):
	m_players(new LP_BASE_PLAYER[MAX_PLAYER_LIMIT])
{
	memset(m_players, 0, sizeof(LP_BASE_PLAYER) * MAX_PLAYER_LIMIT);
}


Players::~Players(void)
{
	delete[] m_players;
}

BASE_PLAYER* Players::login_success(unsigned short& _uid, const char* _key)
{
	PLAYER_LOCK _lock;
	BASE_PLAYER* _PLAYER = NULL;

	if( _uid >= 0 && _uid < MAX_PLAYER_LIMIT )
	{
		BASE_PLAYER* _player = m_players[_uid];
		if( _player != NULL && strcmpi(_key, _player->_KEY) == 0 )
		{
			_player->_PLAYER_ID = _uid;
			_PLAYER = _player;
		}
	}
	else
	{
		int _tempIndex = -1;

		for( unsigned short i=0; i<MAX_PLAYER_LIMIT; i++ )
		{
			BASE_PLAYER* _player = m_players[i];
			if( _tempIndex < 0 && _player == NULL )
			{
				_tempIndex = i;
			}

			if( _player != NULL && strcmpi(_key, _player->_KEY) == 0 )
			{
				_uid = i;
				_player->_PLAYER_ID = i;
				_PLAYER = _player;

				break;
			}
		}

		if( _PLAYER == NULL )
		{
			BASE_PLAYER*& _player = m_players[_tempIndex];
			if( _player == NULL )
			{
				//////////////////////////////////////////////////////////////////////////
				_uid = _tempIndex;

				//////////////////////////////////////////////////////////////////////////
				_player = _ALLOC_PLAYER.createData();
				_player->initData();

				//////////////////////////////////////////////////////////////////////////
				strcpy( _player->_KEY, _key );
				_player->_PLAYER_ID = _tempIndex;
				_PLAYER = _player;
			}
		}
	}

	return _PLAYER;
}

bool Players::refresh_player(unsigned short& _uid, const char* _key)
{
	PLAYER_LOCK _lock;
	bool _check = false;

	if( _uid >= 0 && _uid < MAX_PLAYER_LIMIT )
	{
		BASE_PLAYER* _player = m_players[_uid];
		if( _player != NULL && strcmpi(_key, _player->_KEY) == 0 )
		{
			_check = true;
		}
	}
	else 
	{
		for( int i=0; i<MAX_PLAYER_LIMIT; i++ )
		{
			BASE_PLAYER* _player = m_players[i];
			if( _player != NULL && strcmpi(_key, _player->_KEY) == 0 )
			{
				_uid   = _player->_PLAYER_ID;
				_check = true;
				break;
			}
		}
	}

	return _check;
}

BASE_PLAYER* Players::get_player(unsigned short _uid)
{
	PLAYER_LOCK _lock;
	BASE_PLAYER* _player = NULL;

	if( _uid >= 0 && _uid < MAX_PLAYER_LIMIT )
	{
		_player = m_players[_uid];
	}

	return _player;
}

bool Players::check_player(unsigned short _uid)
{
	PLAYER_LOCK _lock;
	bool _check = false;

	if( _uid >= 0 && _uid < MAX_PLAYER_LIMIT )
	{
		BASE_PLAYER* _player = m_players[_uid];
		if( _player != NULL && _uid == _player->_PLAYER_ID )
		{
			_check = true;
		}
	}

	return _check;
}

void Players::delete_player(unsigned short _uid)
{
	PLAYER_LOCK _lock;

	if( _uid >= 0 && _uid < MAX_PLAYER_LIMIT )
	{
		BASE_PLAYER* _player = m_players[_uid];

		if( _player != NULL )
		{
			m_players[_uid] = NULL;
			_ALLOC_PLAYER.releaseData(_player);
		}
	}
}
