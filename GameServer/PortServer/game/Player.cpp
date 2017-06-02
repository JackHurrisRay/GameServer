#include "Player.h"
#include "./../core/ClientObject.h"
#include "message.h"
#include "json/json.h"
#include "./../core/ClientObject.h"
#include "./../network/NetworkSystem.h"
#include <time.h>

//////////////////////////////////////////////////////////////////////////
ALLOC_PLAYER Players::_ALLOC_PLAYER = ALLOC_PLAYER(MAX_PLAYER_LIMIT, "PLAYER ALLOC");

//////////////////////////////////////////////////////////////////////////
void BASE_PLAYER::initData()
{
	//////////////////////////////////////////////////////////////////////////
	memset(_KEY, 0, sizeof(_KEY));
	memset(_NickName, 0, sizeof(_NickName));
	
	strcpy(_NickName, "JackGame");

	_PLAYER_ID = 0xFFFF;
	_GOLD      = 0;
	_ROOMID    = -1;
	_INDEX     = -1;
	_CLIENT    = NULL;
	_PLAYER_LAST_RESULT = "";

	//////////////////////////////////////////////////////////////////////////
	resetData();
}

void BASE_PLAYER::release()
{

}

#define PLAYER_JSON_DATA "player_data.json"

void BASE_PLAYER::saveData()
{
	////
	//wc_id, gold

	Json::Value _root;

	_root[JSON_PLAYER_KEY]            = _KEY;
	_root[JSON_PLAYER_NICKNAME]       = _NickName;
	_root[JSON_PLAYER_GOLD]           = _GOLD;
	_root[JSON_PLAYER_VIP]            = _EPT_TYPE;
	_root[JSON_PLAYER_VIP_STARTTIME]  = _VIP_START_TIME;
	_root[JSON_MAX_ROOM_CANBECREATED] = _MAX_ROOM_COUNT;
	_root[JSON_LAST_GAMERESULT]       = _PLAYER_LAST_RESULT;


	Json::FastWriter _writer;
	std::string _info = _writer.write(_root);

	std::string _dataPath = _PLAYER_DATA_PATH;
	_dataPath += "\\";
	_dataPath += PLAYER_JSON_DATA;

	JackBase64::writeTextToFile(_dataPath.c_str(), _info);
}

bool BASE_PLAYER::loadData()
{
	bool _check = false;

	std::string _dataPath = _PLAYER_DATA_PATH;
	_dataPath += "\\";
	_dataPath += PLAYER_JSON_DATA;

	////
	std::string _info;

	if( JackBase64::readTextFromFile(_dataPath.c_str(), _info) )
	{
		Json::Value _root;
		Json::Reader _reader;

		if( _reader.parse(_info, _root) )
		{
			//////////////////////////////////////////////////////////////////////////
			//_GOLD = _root[JSON_PLAYER_GOLD].asUInt64();
			_EPT_TYPE = (ENUM_PLAYER_TYPE)_root[JSON_PLAYER_VIP].asUInt64();
			_VIP_START_TIME = _root[JSON_PLAYER_VIP_STARTTIME].asUInt64();
			_MAX_ROOM_COUNT = _root[JSON_MAX_ROOM_CANBECREATED].asUInt64();

			if(!_root[JSON_LAST_GAMERESULT].isNull())
			{
				_PLAYER_LAST_RESULT = _root[JSON_LAST_GAMERESULT].asString();
			}

			if( _MAX_ROOM_COUNT == 0 )
			{
				_MAX_ROOM_COUNT = MAX_ROOM_CANBE_CREATED;
			}

			//std::string _nickname = _root[JSON_PLAYER_NICKNAME].asString();
			//strcpy(_NickName, _nickname.c_str());

			_check = true;
		}
	}

	return _check;
}

bool BASE_PLAYER::isVIP()
{
	const time_t _current_time = time(NULL);
	bool _vip = false;

	switch (_EPT_TYPE)
	{
	case EPT_NONE:

		break;
	case EPT_DAY:
		if( _current_time - _VIP_START_TIME < 24 * 3600 )
		{
			_vip = true;
		}

		break;
	case EPT_WEEK:
		if( _current_time - _VIP_START_TIME < 24 * 3600 * 7 )
		{
			_vip = true;
		}

		break;
	case EPT_MONTH:
		if( _current_time - _VIP_START_TIME < 24 * 3600 * 30 )
		{
			_vip = true;
		}

		break;
	default:

		break;
	}

	return _vip;
}

void BASE_OBJECT::release()
{
	//////////////////////////////////////////////////////////////////////////
	if( _pPerHandle != NULL )
	{
		closesocket(_pPerHandle->s);
		_pPerHandle->s = 0;

		_ALLOC_PER_HANDLE_DATA.releaseData(_pPerHandle);
		_ALLOC_PER_IO_DATA.releaseData(_pPerIo);

		_pPerHandle = NULL;
		_pPerIo     = NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	BASE_PLAYER* _player = Players::Instance()->get_player(_UID);

	if( _player != NULL )
	{
		_player->saveData();
		_player->_CLIENT = NULL;
	}

	lockMSGList();
	_MSG_LIST.clear();
	unlockMSGList();

	cout << "client closed......" << endl;  	
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

int Players::getPlayerCountOnLine()
{
	return _ALLOC_PLAYER.getUsedCound();
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
			_player->release();
			_ALLOC_PLAYER.releaseData(_player);
		}
	}
}

