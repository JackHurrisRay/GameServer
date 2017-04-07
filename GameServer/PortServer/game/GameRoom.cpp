#include "GameRoom.h"
#include "./../core/ClientObject.h"
#include "message.h"
#include "json/json.h"
#include <time.h>

extern unsigned int GET_RAND();

//////////////////////////////////////////////////////////////////////////
struct ROOM_ID_RAND_FLAG
{
	UINT32_LIST _ID_LIST;

	int GET_RAND_FLAG()
	{
		//////////////////////////////////////////////////////////////////////////		
		int _VALUE[6];

		const unsigned int _RAND1 = GET_RAND();

		_VALUE[0] = _RAND1 % 6 + 1;
		_VALUE[1] = _RAND1 % 5;
		_VALUE[2] = GET_RAND() % 10;
		_VALUE[3] = GET_RAND() % 10;
		_VALUE[4] = GET_RAND() % 10;
		_VALUE[5] = GET_RAND() % 10;

		//////////////////////////////////////////////////////////////////////////
		int _number = _VALUE[0] * 100000 + _VALUE[1] * 10000 + _VALUE[2] * 1000 + _VALUE[3] * 100 + _VALUE[4] * 10 + _VALUE[5];

		return _number;
	}

	bool CHECK_REPEAT_FLAG(unsigned short _value)
	{
		UINT32_LIST::iterator _cell;

		bool _check = false;
		for (_cell = _ID_LIST.begin(); _cell != _ID_LIST.end(); _cell++)
		{
			if( _value == *_cell )
			{
				_check = true;
				break;
			}
		}

		return _check;
	}

	ROOM_ID_RAND_FLAG()
	{
		std::cout << "******BEGIN:create room rand ID******" << std::endl;
		for(int i=0; i<MAX_ROOM_LIMIT; i++)
		{
			int _value = 0xFFFFFF;
			while (_value > 0 )
			{
				_value = GET_RAND_FLAG();

				if( !CHECK_REPEAT_FLAG(_value) )
				{
					_ID_LIST.push_back(_value);
					break;
				}
			}
		}
		std::cout << "******END:create room rand ID******" << std::endl;
	}

	~ROOM_ID_RAND_FLAG()
	{

	}

	int POP_VALUE()
	{
		int _value = *_ID_LIST.begin();
		_ID_LIST.pop_front();

		return _value;
	}
};

ROOM_ID_RAND_FLAG _ROOM_RAND_FLAG;

//////////////////////////////////////////////////////////////////////////
ALLOC_ROOM GameRooms::_ALLOC_ROOM = ALLOC_ROOM(MAX_ROOM_LIMIT, "ROOM ALLOC");

//////////////////////////////////////////////////////////////////////////
class ROOM_LOCK
{
public:
	ROOM_LOCK()
	{
		GameRooms::Instance()->m_lock.lock();
	};

	~ROOM_LOCK()
	{
		GameRooms::Instance()->m_lock.unlock();
	};
};

///////////////////////////////////////////////////////////////////////////
BASE_ROOM::BASE_ROOM():
	GAME_DOU_NIU(),
	_ROOM_ID_RANDFLAG(_ROOM_RAND_FLAG.POP_VALUE())
{

}

void BASE_ROOM::initData()
{
	_ROOM_ID       = 0;
	_MAX_ROUND     = 0;
	_BANKER_TYPE   = 0;
	_current_round = 0;
	_start_time    = time(NULL);

	_room_type     = EGRT_NONE;
	_room_status   = ERS_NONE;

	memset(_Players, 0, sizeof(BASE_PLAYER*)*MAX_PLAYER_IN_ROOM);
	memset(_AgreeDisband, 0, sizeof(unsigned char)*MAX_PLAYER_IN_ROOM);

	_OWNER_UID = -1;
	memset(_OWNER_KEY, 0, sizeof(_OWNER_KEY));
	memset(_PASSWORD, 0, sizeof(_PASSWORD));

}

bool BASE_ROOM::isNeedPassword()
{
	bool _check = false;

	for( int i=0; i<MAX_PASSWORD_COUNT; i++ )
	{
		if( _PASSWORD[i] != 0 )
		{
			_check = true;
			break;
		}
	}

	return _check;
}

ENUM_ROOM_ERROR BASE_ROOM::playersAllLeave()
{
	ENUM_ROOM_ERROR _result = ERE_ROOM_OK;

	//////////////////////////////////////////////////////////////////////////
	//所有玩家退出房间
	for( int i=0; i<MAX_PLAYER_IN_ROOM; i++ )
	{
		//////////////////////////////////////////////////////////////////////////
		BASE_PLAYER*& _player = _Players[i];

		if( _player != NULL )
		{
			BASE_OBJECT* _client = _player->_CLIENT;

			if( _client != NULL )
			{
				MSG_S2C_LEAVE_ROOM msg;
				msg._dataLArray[0]->setNumber(0);

				SEND_MSG<MSG_S2C_LEAVE_ROOM>(msg, _client);
			}

			_player->_ROOMID = -1;
			_player = NULL;
		}
	}

	return _result;
}

ENUM_ROOM_ERROR BASE_ROOM::disband()
{
	ENUM_ROOM_ERROR _result = ERE_ROOM_OK;

	bool _checkPlayers = false;
	for( int i=0; i<MAX_PLAYER_IN_ROOM; i++ )
	{
		//////////////////////////////////////////////////////////////////////////
		BASE_PLAYER*& _player = _Players[i];

		if( _player != NULL )
		{
			_checkPlayers = true;
			break;
		}
	}

	if( _checkPlayers )
	{
		return ERE_ROOM_SOMEPLAYERINHERE;
	}

	return _result;
}

int BASE_ROOM::checkPlayerAgreeToLeave()
{
	int _result = 0;

	for( int i=0; i<MAX_PLAYER_IN_ROOM; i++ )
	{
		//////////////////////////////////////////////////////////////////////////
		BASE_PLAYER*& _player = _Players[i];

		if( _player != NULL )
		{
			if( _AgreeDisband[i] == 2 )
			{
				_result = 2;
				break;
			}
			else if( _AgreeDisband[i] == 1 ) 
			{
				_result = 1;
			}
		}
	}

	return _result;
}

void BASE_ROOM::clearAgreeToLeave()
{
	for( int i=0; i<MAX_PLAYER_IN_ROOM; i++ )
	{
		_AgreeDisband[i] = 0;
	}
}

int BASE_ROOM::getCurrentPlayersCount()
{
	int _count = 0;
	for( int i=0; i<MAX_PLAYER_IN_ROOM; i++ )
	{
		if( _Players[i] != NULL )
		{
			_count += 1;
		}
	}

	return _count;
}

void BASE_ROOM::getPlayersInRoom(PLAYER_LIST& _playerList)
{
	for( int i=0; i<MAX_PLAYER_IN_ROOM; i++ )
	{
		BASE_PLAYER* _player = _Players[i];
		if( _player != NULL )
		{
			_playerList.push_back(_player);
		}
	}
}

bool BASE_ROOM::getPlayersInfo(std::string& _info)
{
	Json::Value _root;
	bool _check = false;
	_info = "";

	for( int i=0; i<MAX_PLAYER_IN_ROOM; i++ )
	{
		BASE_PLAYER* _player = _Players[i];
		if( _player != NULL )
		{
			Json::Value _playerData;

			_playerData[JSON_PLAYER_UID] = _player->_PLAYER_ID;
			_playerData[JSON_PLAYER_KEY] = _player->_KEY;
			_playerData[JSON_PLAYER_NICKNAME] = _player->_NickName;
			_playerData[JSON_PLAYER_INDEXINROOM] = _player->_INDEX;
			_playerData[JSON_PLAYER_GAMESTATUS]  = _player->_status;
			_playerData[JSON_ZHUANG]             = _player->_isZhuang;

			_root[JSON_PLAYER].append(_playerData);
		}

		_check = true;
	}

	if( _check && _zhuangPlayer != NULL )
	{
		_root[JSON_ZHUANG_VALUE] = _zhuangValue;
	}

	if( _check )
	{
		Json::FastWriter _writer;
		_info = _writer.write(_root);
	}

	return _check;

}

/************************************************************************/
/*                                                                      */
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
GameRooms::GameRooms(void):
	m_rooms(new LP_BASE_ROOM[MAX_ROOM_LIMIT])
{
	memset(m_rooms, 0, sizeof(LP_BASE_ROOM) * MAX_ROOM_LIMIT);
}


GameRooms::~GameRooms(void)
{
	delete[] m_rooms;
}

BASE_ROOM* GameRooms::get_room(short room_id)
{
	BASE_ROOM* _room = NULL;

	if( room_id >= 0 && room_id < MAX_ROOM_LIMIT )
	{
		_room = m_rooms[room_id];
	}

	return _room;
}

//创建房间 0:成功创建房间 1:金币不足无法创建房间 2:条件不足无法创建房间 3:房间已满，无法创建房间       >0xFF:其他原因无法创建房间
ENUM_ROOM_ERROR GameRooms::createRoom(BASE_PLAYER* _player, BASE_ROOM*& _room)
{
	ROOM_LOCK _lock;

	ENUM_ROOM_ERROR _result = ERE_ROOM_UNKNOWN;

	for (int i=0; i<MAX_ROOM_LIMIT; i++)
	{
		BASE_ROOM*& _roomChoose = m_rooms[i];

		if( _roomChoose == NULL )
		{
			//////////////////////////////////////////////////////////////////////////
			_roomChoose = _ALLOC_ROOM.createData();
			_roomChoose->initData();

			//////////////////////////////////////////////////////////////////////////
			_roomChoose->_ROOM_ID = i;

			_roomChoose->_OWNER_UID = _player->_PLAYER_ID;
			strcpy(_roomChoose->_OWNER_KEY, _player->_KEY);

			//////////////////////////////////////////////////////////////////////////
			_room = _roomChoose;
			_result = ERE_ROOM_OK;

			break;
		}
	}

	return _result;	
}

//进入房间 0:成功进入房间 1:房间已满，无法进入房间 2:条件不足无法进入房间 3:已经在房间内，无法进入新房间 0xF:房间不存在无法进入 >0xFF:其他原因无法进入房间
ENUM_ROOM_ERROR GameRooms::enterRoom(int _rand_key, BASE_PLAYER* _player, const char* _password, BASE_ROOM*& _room)
{
	ROOM_LOCK _lock;

	ENUM_ROOM_ERROR _result = ERE_ROOM_UNKNOWN;
	_room = NULL;
	
	for( int i=0; i<MAX_ROOM_LIMIT; i++ )
	{
		if( m_rooms[i] != NULL && m_rooms[i]->_ROOM_ID_RANDFLAG == _rand_key )
		{
			_room = m_rooms[i];
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if( _room == NULL )
	{
		return ERE_ROOM_IS_NOT_EXIST;//房间不存在
	}

	if(_room->_room_status == ERS_INGAME )
	{
		return ERE_ROOM_IN_GAME;
	}

	//////////////////////////////////////////////////////////////////////////
	if( _player->_ROOMID > 0 && _player->_ROOMID != _room->_ROOM_ID )
	{
		return ERE_ROOM_PLAYER_IN_OTHER_ROOM;//玩家本身存在与其他房间内
	}

	//////////////////////////////////////////////////////////////////////////
	if( _player->_ROOMID >= 0 && _player->_ROOMID == _room->_ROOM_ID )
	{
		BASE_PLAYER* _findPlayer = NULL;
		for( int i=0; i<MAX_PLAYER_IN_ROOM; i++ )
		{
			BASE_PLAYER* _room_player = _room->_Players[i];

			if( _room_player == _player )
			{
				_findPlayer = _room_player;
				break;
			}
		}

		if( _findPlayer != NULL )
		{
			return ERE_ROOM_PLAYER_IN_HERE;//玩家本来就存在于该房间内
		}		
	}

	//////////////////////////////////////////////////////////////////////////
	if( _room->_room_status == ERS_INGAME )
	{
		return ERE_ROOM_IN_GAME;
	}

	if( _room->isNeedPassword() )
	{
		if( strcmp(_room->_PASSWORD, _password) != 0 )
		{
			return ERE_ROOM_WRONG_PASSWORD;
		}
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//////////////////////////////////////////////////////////////////////////
	int _player_index_in_room = -1;
	for( int i=0; i<MAX_PLAYER_IN_ROOM; i++ )
	{
		BASE_PLAYER* _room_player = _room->_Players[i];

		if( _room_player == NULL )
		{
			//有位置可以进入
			_player_index_in_room = i;
		}
	}

	if( _player_index_in_room >= 0 )
	{
		_room->_Players[_player_index_in_room] = _player;
		_player->_ROOMID     = _room->_ROOM_ID;
		_player->_INDEX      = _player_index_in_room;
		_player->_totalSCORE = 0;

		//////////////////////////////////////////////////////////////////////////
		_player->_status = EPS_NONE;

		_result = ERE_ROOM_OK;
	}
	else
	{
		_result = ERE_ROOM_IS_FULL;
	}

	return _result;	
}

//离开房间 0:成功离开房间 1:游戏中无法离开房间 >0xF:其他原因无法离开房间 0xF:房间不存在无法离开 >0xFF:其他原因无法离开
ENUM_ROOM_ERROR GameRooms::leaveRoom(BASE_PLAYER* _player, BASE_ROOM*& _room)
{
	ROOM_LOCK _lock;

	ENUM_ROOM_ERROR _result = ERE_ROOM_UNKNOWN;

	const int   _room_id      = _player->_ROOMID;
	const int   _player_index = _player->_INDEX;

	//////////////////////////////////////////////////////////////////////////
	_room = NULL;

	if( _room_id >= 0 && _room_id < MAX_ROOM_LIMIT && _player_index >= 0 && _player_index < MAX_PLAYER_IN_ROOM )
	{
		_room = m_rooms[_room_id];
	}

	//////////////////////////////////////////////////////////////////////////
	if( _room == NULL )
	{
		//房间不存在，玩家需要数据重置
		_player->_ROOMID = -1;
		_player->_INDEX  = -1;

		return ERE_ROOM_IS_NOT_EXIST;//房间不存在
	}

	//////////////////////////////////////////////////////////////////////////
	if( _room->_room_status == ERS_INGAME )
	{
		return ERE_ROOM_IN_GAME;
	}

	if( _room->_Players[_player_index] != _player )
	{
		return ERE_ROOM_UNKNOWN;
	}

	//////////////////////////////////////////////////////////////////////////
	//离开房间
	_room->_Players[_player_index] = NULL;
	_player->_ROOMID = -1;
	_player->_INDEX  = -1;

	//////////////////////////////////////////////////////////////////////////
	_player->_status = EPS_NONE;

	_result = ERE_ROOM_OK;

	return _result;	
}

//全部离开房间 0:成功解散房间 1:有人不同意解散房间 2:房间状态不允许解散房间 0xF:房间不存在无法解散 >0xFF:其他原因无法解散房间
ENUM_ROOM_ERROR GameRooms::leaveGameRoom(bool _agree, BASE_PLAYER* _player, BASE_ROOM*& _room)
{
	ROOM_LOCK _lock;

	ENUM_ROOM_ERROR _result = ERE_ROOM_UNKNOWN;

	const int   _room_id      = _player->_ROOMID;
	const int   _player_index = _player->_INDEX;

	//////////////////////////////////////////////////////////////////////////
	_room = NULL;

	if( _room_id >= 0 && _room_id < MAX_ROOM_LIMIT && _player_index >= 0 && _player_index < MAX_PLAYER_IN_ROOM )
	{
		_room = m_rooms[_room_id];
	}

	//////////////////////////////////////////////////////////////////////////
	if( _room == NULL )
	{
		//房间不存在，玩家需要数据重置
		_player->_ROOMID = -1;
		_player->_INDEX  = -1;

		return ERE_ROOM_IS_NOT_EXIST;//房间不存在
	}

	//////////////////////////////////////////////////////////////////////////
	if( _room->_Players[_player_index] != _player )
	{
		return ERE_ROOM_UNKNOWN;
	}

	//////////////////////////////////////////////////////////////////////////


	return _result;	
}

ENUM_ROOM_ERROR GameRooms::disbandRoomByOwner(short _room_id, BASE_PLAYER* _player)
{
	ENUM_ROOM_ERROR _result = ERE_ROOM_UNKNOWN;

	//////////////////////////////////////////////////////////////////////////
	BASE_ROOM* _room = get_room(_room_id);

	if( _room == NULL)
	{
		return ERE_ROOM_IS_NOT_EXIST;
	}
	else if( _room->_OWNER_UID != _player->_PLAYER_ID )
	{
		return ERE_ROOM_LIMITRIGHT;
	}
	else if( _room->getCurrentPlayersCount() > 0 )
	{
		return ERE_ROOM_SOMEPLAYERINHERE;
	}
	else if( _room->_room_status == ERS_INGAME )
	{
		return ERE_ROOM_WRONG_STATUS;
	}

	//////////////////////////////////////////////////////////////////////////
	if( _room_id >= 0 && _room_id < MAX_ROOM_LIMIT )
	{
		BASE_ROOM*& _room = m_rooms[_room_id];

		if( _room != NULL && _room->_OWNER_UID == _player->_PLAYER_ID )
		{
			BASE_ROOM* _delRoom = _room;
			_room   = NULL;
			_result = ERE_ROOM_OK;

			_ALLOC_ROOM.releaseData(_delRoom);
		}
	}

	return _result;
}

void GameRooms::updateRooms()
{
	ROOM_LOCK _lock;

	for( int i=0; i<MAX_ROOM_LIMIT; i++ )
	{
		BASE_ROOM* _room = m_rooms[i];

		if( _room != NULL )
		{

		}
	}
}

void GameRooms::getRoomsByOwner(const unsigned short _player_id, ROOM_LIST& _room_list)
{
	ROOM_LOCK _lock;

	for( int i=0; i<MAX_ROOM_LIMIT; i++ )
	{
		BASE_ROOM* _room = m_rooms[i];

		if( _room != NULL && _room->_OWNER_UID == _player_id )
		{
			_room_list.push_back(_room);
		}
	}
}

