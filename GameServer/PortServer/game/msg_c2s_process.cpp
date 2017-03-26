#include "msg_c2s_process.h"
#include "game_common.h"
#include "json/json.h"

//////////////////////////////////////////////////////////////////////////
#include "protocal.h"
#include "message.h"
#include "./../core/ClientObject.h"

//////////////////////////////////////////////////////////////////////////
#include "Player.h"
#include "GameRoom.h"

//////////////////////////////////////////////////////////////////////////
FACTORY_BEGIN::FACTORY_BEGIN()
{
	//////////////////////////////////////////////////////////////////////////
	Players::Instance();
	GameRooms::Instance();

	//////////////////////////////////////////////////////////////////////////
#define BIND_CALLBACK(X) \
CProtocalFactory::Instance()->bind_func(ENUM_GAME_PROTOCAL::EGP_##X, &NET_CALLBACK_##X)

	//////////////////////////////////////////////////////////////////////////
	std::cout << "...init protocal..." << std::endl;

	BIND_CALLBACK(C2S_LOGIN);
	BIND_CALLBACK(C2S_CREATE_ROOM);

	BIND_CALLBACK(C2S_ENTER_ROOM);
	BIND_CALLBACK(C2S_LEAVE_ROOM);

	BIND_CALLBACK(C2S_DISBAND_ROOM_BY_OWNER);
	BIND_CALLBACK(C2S_REQUEST_ROOMLIST);
	//////////////////////////////////////////////////////////////////////////

};

const FACTORY_BEGIN  _FB;

ENUM_PROTOCAL_ERROR CHECK_PLAYER_FIT(BASE_OBJECT* client)
{	
	ENUM_PROTOCAL_ERROR _status = PROTOCAL_ERROR_UNKNOW;

	if( !Players::Instance()->check_player(client->_UID) )
	{
		_status = PROTOCAL_ERROR_NONELOGIN;
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		_status = PROTOCAL_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	if( _status != PROTOCAL_OK )
	{
		MSG_PROTOCAL_ERROR _msg(_status);
		SEND_MSG<MSG_PROTOCAL_ERROR>(_msg, client);
	}

	return _status;
}

#define CHECK_MSG_PARAM(X) \
if(!CHECK_PARAM<X>(data)){return false;}

#define  CHECK_MSG_LOGIN(X) \
if( CHECK_PLAYER_FIT(X) != PROTOCAL_OK ){return true;}

#define  GET_PLAYER \
BASE_PLAYER* player = Players::Instance()->get_player(client->_UID);

/************************************************************************/
/* 协议解析                                                              */
/************************************************************************/
NET_CALLBACK(C2S_LOGIN)
{
	CHECK_MSG_PARAM(MSG_C2S_LOGIN);

	//////////////////////////////////////////////////////////////////////////
	std::string _player_key = data[JSON_PLAYER_KEY].asString();
	std::string _player_img = data[JSON_PLAYER_IMG].asString();

	//////////////////////////////////////////////////////////////////////////
	//GAME_LOG("PLAYER_KEY:" << _player_key, true);
	//GAME_LOG("PLAYER_IMG:" << _player_img, true);

	BIG_BUFF* _tempBuff = _ALLOC_BIG_BUFF.createData();
	_tempBuff->reset();

	char* _buff = _tempBuff->_buff;
	int _img_len = JackBase64::GetInterNetURLText(_player_img.c_str(), _buff, BUFFER_BIG_SIZE);

	if( _img_len > 0 )
	{
		//////////////////////////////////////////////////////////////////////////
		std::string _player_img_path = JackBase64::GAME_CONFIG::Instance()->_SOURCE_PATH;
		_player_img_path += "\\";
		_player_img_path += _player_key;

		JackBase64::checkPath(_player_img_path.c_str());

		_player_img_path += "\\img_account";

		JackBase64::writefile(_player_img_path.c_str(), _buff, _img_len);

	}

	_ALLOC_BIG_BUFF.releaseData(_tempBuff);

	//////////////////////////////////////////////////////////////////////////
	unsigned short _uid = 0xFFFF;

	//此处需要插入微信登录验证
	BASE_PLAYER* _player = Players::Instance()->login_success(_uid, _player_key.c_str());
	if( _player != NULL )
	{
		//////////////////////////////////////////////////////////////////////////
		client->_UID = _uid;
		_player->_CLIENT = client;

		//////////////////////////////////////////////////////////////////////////
		_player->_GOLD = 1000;

		//////////////////////////////////////////////////////////////////////////
		MSG_S2C_LOGIN _msg;
		_msg._dataLArray[0]->setNumber(_uid);
		_msg._dataLArray[1]->setNumber(_player->_GOLD);
		_msg._dataLArray[2]->setNumber(_player->_ROOMID);
		_msg._dataLArray[3]->setString(_player->_KEY);

		////
		_msg._dataLArray[4]->setString(JackBase64::GAME_CONFIG::Instance()->_JSON_DATA_FOR_GAME.c_str());

		SEND_MSG<MSG_S2C_LOGIN>(_msg, client);

		//////////////////////////////////////////////////////////////////////////
		_player->_PLAYER_DATA_PATH = JackBase64::GAME_CONFIG::Instance()->_PLAYER_PATH;
		_player->_PLAYER_DATA_PATH += _player_key;

		JackBase64::checkPath(_player->_PLAYER_DATA_PATH.c_str());
	}
	else
	{
		//////////////////////////////////////////////////////////////////////////
		//登录失败
		MSG_S2C_LOGIN _msg;
		_msg._error_code = PROTOCAL_ERROR_LOGINFAILED;
		SEND_MSG<MSG_S2C_LOGIN>(_msg, client);
	}

	return true;
}

NET_CALLBACK(C2S_CREATE_ROOM)
{
	//////////////////////////////////////////////////////////////////////////
	CHECK_MSG_PARAM(MSG_C2S_CREATE_ROOM);
	CHECK_MSG_LOGIN(client);
	GET_PLAYER;

	//////////////////////////////////////////////////////////////////////////
	int _room_type = data[JSON_ROOM_TYPE].asInt();
	if( _room_type > EGRT_NONE && _room_type < EGRT_COUNT )
	{

	}
	else
	{
		return false;
	}

	int _room_around_count = data[JSON_ROOM_MAX_AROUND].asInt();
	if( _room_around_count >= 0 && _room_around_count < 3 )
	{

	}
	else
	{
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	BASE_ROOM* _room = NULL;
	ENUM_ROOM_ERROR _hr = GameRooms::Instance()->createRoom(player, _room);

	//若有密码，则需要设置密码
	std::string _PASSWORD = data[JSON_PASSWORD].asString();

	if( _hr == ERE_ROOM_OK &&  _PASSWORD != "0" )
	{
		if( _PASSWORD.size() >= MAX_PASSWORD_COUNT - 2 && _PASSWORD.size() < MAX_PASSWORD_COUNT )
		{

		}
		else
		{
			_hr = ERE_ROOM_PASSWORD_TYPE;
		}
	}

	if( _hr == ERE_ROOM_OK )
	{
		//////////////////////////////////////////////////////////////////////////
		if( _PASSWORD != "0" )
		{
			strcpy(_room->_PASSWORD, _PASSWORD.c_str());
		}

		//////////////////////////////////////////////////////////////////////////
		_room->_room_type = (ENUM_GAME_ROOM_TYPE)_room_type;
		_room->_room_status = ERS_NOGAME;

		const int _MAX_AROUND_COUNT[] = {10, 20, 30};
		_room->_MAX_ROUND = _MAX_AROUND_COUNT[_room_around_count];

		//////////////////////////////////////////////////////////////////////////
		MSG_S2C_CREATE_ROOM _msg;
		_msg._dataLArray[0]->setNumber(_room->_ROOM_ID);
		_msg._dataLArray[1]->setNumber(_room->_ROOM_ID_RANDFLAG);
		_msg._dataLArray[2]->setNumber(_room->_room_type);
		_msg._dataLArray[3]->setNumber(_room->_room_status);

		SEND_MSG<MSG_S2C_CREATE_ROOM>(_msg, client);
	}
	else
	{
		MSG_S2C_CREATE_ROOM _msg;
		_msg._error_code = PROTOCAL_ERROR_ROOM;
		_msg._error_ex   = _hr;
		SEND_MSG<MSG_S2C_CREATE_ROOM>(_msg, client);
	}

	return true;
}

NET_CALLBACK(C2S_ENTER_ROOM)
{
	//////////////////////////////////////////////////////////////////////////
	CHECK_MSG_PARAM(MSG_C2S_ENTER_ROOM);
	CHECK_MSG_LOGIN(client);
	GET_PLAYER;

	const short _room_id = data[JSON_ROOM_ID].asInt();
	const std::string _password = data[JSON_PASSWORD].asString();

	BASE_ROOM* _room;
	ENUM_ROOM_ERROR _hr = GameRooms::Instance()->enterRoom(_room_id, player, _password.c_str(), _room);

	if( _hr == ERE_ROOM_OK )
	{
		MSG_S2C_ENTER_ROOM _msg;
		_msg._dataLArray[0]->setNumber(_room_id);
		_msg._dataLArray[1]->setNumber(player->_PLAYER_ID);

		//////////////////////////////////////////////////////////////////////////
		_room->brodcast<MSG_S2C_ENTER_ROOM>(_msg);
	}
	else
	{
		MSG_S2C_ENTER_ROOM _msg;
		_msg._error_code = PROTOCAL_ERROR_ROOM;
		_msg._error_ex   = _hr;
		SEND_MSG<MSG_S2C_ENTER_ROOM>(_msg, client);
	}

	return true;
}

NET_CALLBACK(C2S_LEAVE_ROOM)
{
	//////////////////////////////////////////////////////////////////////////
	CHECK_MSG_PARAM(MSG_C2S_LEAVE_ROOM);
	CHECK_MSG_LOGIN(client);
	GET_PLAYER;

	BASE_ROOM* _room;
	ENUM_ROOM_ERROR _hr = GameRooms::Instance()->leaveRoom(player, _room);

	if( _hr == ERE_ROOM_OK )
	{
		MSG_S2C_LEAVE_ROOM _msg;
		_msg._dataLArray[0]->setNumber(player->_PLAYER_ID);
		_msg._dataLArray[1]->setNumber(_room->_ROOM_ID);

		//////////////////////////////////////////////////////////////////////////
		_room->brodcast<MSG_S2C_LEAVE_ROOM>(_msg);
		SEND_MSG<MSG_S2C_LEAVE_ROOM>(_msg, client);

	}
	else
	{
		MSG_S2C_LEAVE_ROOM _msg;
		_msg._error_code = PROTOCAL_ERROR_ROOM;
		_msg._error_ex   = _hr;
		SEND_MSG<MSG_S2C_LEAVE_ROOM>(_msg, client);
	}

	return true;
}

NET_CALLBACK(C2S_DISBAND_ROOM_BY_OWNER)
{
	//////////////////////////////////////////////////////////////////////////
	CHECK_MSG_PARAM(MSG_C2S_DISBAND_ROOM_BY_OWNER);
	CHECK_MSG_LOGIN(client);
	GET_PLAYER;

	const short _room_id = data[JSON_ROOM_ID].asInt();

	//////////////////////////////////////////////////////////////////////////
	//房间可以被解散掉
	ENUM_ROOM_ERROR _hr = GameRooms::Instance()->disbandRoomByOwner(_room_id, player);

	if( _hr == ERE_ROOM_OK )
	{
		MSG_S2C_DISBAND_ROOM_BY_OWNER _msg;
		_msg._dataLArray[0]->setNumber(_room_id);
		SEND_MSG<MSG_S2C_DISBAND_ROOM_BY_OWNER>(_msg, client);
	}
	else
	{
		MSG_S2C_DISBAND_ROOM_BY_OWNER _msg;
		_msg._error_code = PROTOCAL_ERROR_ROOM;
		_msg._error_ex   = _hr;
		SEND_MSG<MSG_S2C_DISBAND_ROOM_BY_OWNER>(_msg, client);
	}

	return true;
}

NET_CALLBACK(C2S_REQUEST_ROOMLIST)
{
	//////////////////////////////////////////////////////////////////////////
	CHECK_MSG_PARAM(MSG_C2S_REQUEST_ROOMLIST);
	CHECK_MSG_LOGIN(client);
	GET_PLAYER;

	const unsigned short _UID = player->_PLAYER_ID;

	ROOM_LIST _room_list;
	GameRooms::Instance()->getRoomsByOwner(_UID, _room_list);

	//////////////////////////////////////////////////////////////////////////
	MSG_S2C_REQUEST_ROOMLIST _msg;

	//////////////////////////////////////////////////////////////////////////
	INTEGER_ARRAY _ROOM_ID_ARRAY;
	INTEGER_ARRAY _ROOM_RAND_ID_ARRAY;
	INTEGER_ARRAY _ROOM_PLAYERSCOUNT_ARRAY;

	//////////////////////////////////////////////////////////////////////////
	for(ROOM_LIST::iterator cell = _room_list.begin(); cell != _room_list.end(); cell++ )
	{
		BASE_ROOM* _room = *cell;

		if( _room != NULL )
		{
			_ROOM_ID_ARRAY.push_back(_room->_ROOM_ID);
			_ROOM_RAND_ID_ARRAY.push_back(_room->_ROOM_ID_RANDFLAG);
			_ROOM_PLAYERSCOUNT_ARRAY.push_back(_room->getCurrentPlayersCount());
		}
	}

	if( _ROOM_ID_ARRAY.size() > 0 )
	{
		_msg._dataLArray[0]->setIArray(_ROOM_ID_ARRAY);
		_msg._dataLArray[1]->setIArray(_ROOM_RAND_ID_ARRAY);
		_msg._dataLArray[2]->setIArray(_ROOM_PLAYERSCOUNT_ARRAY);
	}

	SEND_MSG<MSG_S2C_REQUEST_ROOMLIST>(_msg, client);

	return true;
}
