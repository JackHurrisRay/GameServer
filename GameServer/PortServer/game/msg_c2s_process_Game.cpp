#include "msg_c2s_process.h"
#include "game_common.h"
#include "json/json.h"
#include <time.h>

//////////////////////////////////////////////////////////////////////////
#include "protocal.h"
#include "message.h"
#include "message_douniu.h"

#include "./../core/utilcore.h"
#include "./../core/ClientObject.h"

//////////////////////////////////////////////////////////////////////////
#include "Player.h"
#include "GameRoom.h"

bool CHECK_MSG_PLAYER_IN_ROOM(BASE_PLAYER* player, BASE_ROOM*& _room)
{
	_room = NULL;

	if( player == NULL )
	{
		return false;
	}

	const short _room_id = player->_ROOMID;
	_room = GameRooms::Instance()->get_room(_room_id);

	if( _room == NULL )
	{
		return false;
	}

	if( _room->_ROOM_ID != _room_id )
	{
		return false;
	}

	return _room != NULL;
}

#define GET_ROOM \
BASE_ROOM* room = NULL;\
if(!CHECK_MSG_PLAYER_IN_ROOM(player, room)){return false;}

void REFRESH_ROOM_GAMESTATUS(BASE_ROOM* room, int LIMIT_COUNT=0)
{
	const ENUM_PLAYER_STATUS _statusOld = room->_PLAYERS_STATUS;
	const ENUM_PLAYER_STATUS _statusNew = (ENUM_PLAYER_STATUS)(room->_PLAYERS_STATUS + 1);

	int  _count = 0;
	bool _check = true;
	for( int i=0; i<MAX_PLAYER_IN_ROOM; i++ )
	{
		if( room->_Players[i] != NULL )
		{
			if( room->_Players[i]->_status == _statusNew )
			{
				_count += 1;
			}
			else
			{
				_check = false;
				break;
			}
		}
	}

	if( _check && _count > LIMIT_COUNT )
	{
		room->_PLAYERS_STATUS = _statusNew;

		if( room->_PLAYERS_STATUS == EPS_MAX )
		{
			room->_PLAYERS_STATUS = EPS_NONE;
		}
	}

}

#define  CHECK_ROOM_STATUS(X) if(room->_PLAYERS_STATUS!=X){return false;}

NET_CALLBACK(C2S_READYGAME)
{
	//////////////////////////////////////////////////////////////////////////
	CHECK_MSG_PARAM(MSG_C2S_READYGAME);
	CHECK_MSG_LOGIN(client);
	GET_PLAYER;
	GET_ROOM;
	CHECK_ROOM_STATUS(EPS_NONE);

	//////////////////////////////////////////////////////////////////////////
	ENUM_GAME_STATUS_ERROR _hr = player->readyGame();

	if( _hr == EGSE_OK )
	{
		////////
		REFRESH_ROOM_GAMESTATUS(room, 1);

		if( room->_PLAYERS_STATUS == EPS_READY )
		{
			//游戏开始
			room->_room_status   = ERS_INGAME;
			room->_zhuangPlayer  = NULL;
			room->_currentRound += 1;

			//洗牌
			room->_PokeCard->Shuffle_cards();
			room->_INFO_playersPokeCard = "0";

			CPokeCard* _pokeCard = room->_PokeCard;

			//////////////////////////////////////////////////////////////////////////
			POKE_LIST _pokeList;
			_pokeCard->get_card_list(_pokeList);

			//////////////////////////////////////////////////////////////////////////
			//玩家列表
			PLAYER_LIST _playerList;
			room->getPlayersInRoom(_playerList);


			//////////////////////////////////////////////////////////////////////////
			//预先设置牌型
			int _prepareIndex = 0;

			

			//////////////////////////////////////////////////////////////////////////
			//发牌
			for( PLAYER_LIST::iterator cell = _playerList.begin(); cell != _playerList.end(); cell++ )
			{
				BASE_PLAYER* _playerData = *cell;

				for( int i=0; i<MAX_CARD_PER_PLAYER; i++ )
				{
					_playerData->_card[i] = (*_pokeList.begin());
					_pokeList.pop_front();
				}
			}
		}

		////////
		MSG_S2C_ALL_READYGAME _msg;
		_msg._dataLArray[0]->setNumber(player->_PLAYER_ID);
		_msg._dataLArray[1]->setString(player->_KEY);
		_msg._dataLArray[2]->setNumber(player->_INDEX);

		_msg._dataLArray[3]->setNumber(player->_status);
		_msg._dataLArray[4]->setNumber(room->_PLAYERS_STATUS);

		_msg._dataLArray[5]->setNumber(room->_currentRound);
		_msg._dataLArray[6]->setNumber(room->_MAX_ROUND);

		room->brodcast<MSG_S2C_ALL_READYGAME>(_msg, player);
		SEND_MSG<MSG_S2C_ALL_READYGAME>(_msg, client);
	}
	else
	{
		MSG_S2C_ALL_READYGAME _msg;
		_msg._error_code = PROTOCAL_ERROR_GAME;
		_msg._error_ex   = _hr;
		SEND_MSG<MSG_S2C_ALL_READYGAME>(_msg, client);
	}

	return true;
};

NET_CALLBACK(C2S_CANCELREADY)
{
	//////////////////////////////////////////////////////////////////////////
	CHECK_MSG_PARAM(MSG_C2S_CANCELREADY);
	CHECK_MSG_LOGIN(client);
	GET_PLAYER;
	GET_ROOM;
	CHECK_ROOM_STATUS(EPS_NONE);

	//////////////////////////////////////////////////////////////////////////
	ENUM_GAME_STATUS_ERROR _hr = player->cancelReady();

	if( _hr == EGSE_OK )
	{
		MSG_S2C_ALL_CANCELREADY _msg;
		_msg._dataLArray[0]->setNumber(player->_PLAYER_ID);
		_msg._dataLArray[1]->setString(player->_KEY);
		_msg._dataLArray[2]->setNumber(player->_INDEX);

		_msg._dataLArray[3]->setNumber(player->_status);
		_msg._dataLArray[4]->setNumber(room->_PLAYERS_STATUS);

		room->brodcast<MSG_S2C_ALL_CANCELREADY>(_msg, player);
		SEND_MSG<MSG_S2C_ALL_CANCELREADY>(_msg, client);
	}
	else
	{
		MSG_C2S_CANCELREADY _msg;
		_msg._error_code = PROTOCAL_ERROR_GAME;
		_msg._error_ex   = _hr;
		SEND_MSG<MSG_C2S_CANCELREADY>(_msg, client);
	}

	return true;
};

NET_CALLBACK(C2S_FIGHT_ZHUANG)
{
	//////////////////////////////////////////////////////////////////////////
	CHECK_MSG_PARAM(MSG_C2S_CANCELREADY);
	CHECK_MSG_LOGIN(client);
	GET_PLAYER;
	GET_ROOM;
	CHECK_ROOM_STATUS(EPS_READY);

	//////////////////////////////////////////////////////////////////////////
	const int _zhuangValue = data[JSON_ZHUANG_VALUE].asInt();

	if( _zhuangValue <= 0 || _zhuangValue > 255 )
	{
		return false;
	}

	ENUM_GAME_STATUS_ERROR _hr = player->fightForZhuang(_zhuangValue);

	if( _hr == EGSE_OK )
	{
		REFRESH_ROOM_GAMESTATUS(room);
		std::string _EX_DATA = "0";

		//判定进入下一个环节
		if( room->_PLAYERS_STATUS == EPS_FIGHT_FOR_ZHUANG )
		{
			//////////////////////////////////////////////////////////////////////////
			//知道谁是庄
			int _maxZhuang = 0;
			PLAYER_LIST _playerList;
			room->getPlayersInRoom(_playerList);

			for( PLAYER_LIST::iterator cell = _playerList.begin(); cell != _playerList.end(); cell++ )
			{
				BASE_PLAYER* _playerData = *cell;
				if( _maxZhuang < _playerData->_zhuang )
				{
					_maxZhuang = _playerData->_zhuang;
				}
			}

			std::vector<BASE_PLAYER*> _playerArray;
			for( PLAYER_LIST::iterator cell = _playerList.begin(); cell != _playerList.end(); cell++ )
			{
				BASE_PLAYER* _playerData = *cell;
				if( _maxZhuang == _playerData->_zhuang )
				{
					_playerArray.push_back(_playerData);
				}
			}

			const int _zhuangCount = _playerArray.size();
			BASE_PLAYER* _zhuangPlayer = NULL;

			if( _zhuangCount > 0 )
			{
				_zhuangPlayer = _playerArray[0];
			}

			if( _zhuangCount > 1 )
			{
				const int _zhuangIndex = GET_RAND() % _zhuangCount;
				_zhuangPlayer = _playerArray[_zhuangIndex];
			}

			//////////////////////////////////////////////////////////////////////////
			if( _zhuangPlayer != NULL )
			{
				//确定庄家
				room->_zhuangPlayer = _zhuangPlayer;
				room->_zhuangValue  = _zhuangPlayer->_zhuang;

				_zhuangPlayer->_isZhuang = true;

				//////////////////////////////////////////////////////////////////////////
				if( GAME_DOU_NIU::getPlayerPokeCardInfo(room, _EX_DATA) )
				{
					room->_INFO_playersPokeCard = _EX_DATA;
				}
			}
		}

		{
			MSG_S2C_ALL_FIGHT_ZHUANG _msg;
			_msg._dataLArray[0]->setNumber(player->_PLAYER_ID);
			_msg._dataLArray[1]->setString(player->_KEY);

			_msg._dataLArray[2]->setNumber(player->_status);
			_msg._dataLArray[3]->setNumber(room->_PLAYERS_STATUS);

			_msg._dataLArray[4]->setNumber(player->_zhuang);
			_msg._dataLArray[5]->setString(_EX_DATA.c_str());
			_msg._dataLArray[6]->setNumber(0);

			room->brodcast<MSG_S2C_ALL_FIGHT_ZHUANG>(_msg, player);

		}

		{
			MSG_S2C_ALL_FIGHT_ZHUANG _msg;
			_msg._dataLArray[0]->setNumber(player->_PLAYER_ID);
			_msg._dataLArray[1]->setString(player->_KEY);

			_msg._dataLArray[2]->setNumber(player->_status);
			_msg._dataLArray[3]->setNumber(room->_PLAYERS_STATUS);

			_msg._dataLArray[4]->setNumber(player->_zhuang);
			_msg._dataLArray[5]->setString(_EX_DATA.c_str());

			INTEGER_ARRAY _pokecard_data;
			player->getPokeCard(_pokecard_data);

			if( _pokecard_data.size() != 5 )
			{
				//some error
				GAME_LOG("****** pokecard error! ******", true);
			}

			_msg._dataLArray[6]->setIArray(_pokecard_data);
			SEND_MSG<MSG_S2C_ALL_FIGHT_ZHUANG>(_msg, client);
		}
	}
	else
	{
		MSG_S2C_ALL_FIGHT_ZHUANG _msg;
		_msg._error_code = PROTOCAL_ERROR_GAME;
		_msg._error_ex   = _hr;
		SEND_MSG<MSG_S2C_ALL_FIGHT_ZHUANG>(_msg, client);
	}

	return true;
}

NET_CALLBACK(C2S_ADD_DOUBLE)
{
	//////////////////////////////////////////////////////////////////////////
	CHECK_MSG_PARAM(MSG_C2S_ADD_DOUBLE);
	CHECK_MSG_LOGIN(client);
	GET_PLAYER;
	GET_ROOM;
	CHECK_ROOM_STATUS(EPS_FIGHT_FOR_ZHUANG);

	//////////////////////////////////////////////////////////////////////////
	const int _doubleValue = data[JSON_DOUBLE].asInt();

	if( _doubleValue < 0 || _doubleValue > 255 )
	{
		return false;
	}

	ENUM_GAME_STATUS_ERROR _hr = player->doubleScore(_doubleValue);

	if( _hr == EGSE_OK )
	{
		//////////////////////////////////////////////////////////////////////////
		player->checkWinCard();

		//////////////////////////////////////////////////////////////////////////
		REFRESH_ROOM_GAMESTATUS(room);
		std::string _EX_DATA = "0";

		bool _lastAround = false;
		if( room->_PLAYERS_STATUS == EPS_DEALER )
		{
			//////////////////////////////////////////////////////////////////////////
			//加倍完成，开始计算结果
			GAME_DOU_NIU::computerPlayerScore(room, _EX_DATA);

			//如果是最后一局，则出现总结算，并且退出房间
			if( room->_currentRound == room->_MAX_ROUND )
			{
				_lastAround = true;
			}
		}

		MSG_S2C_ALL_ADD_DOUBLE _msg;
		_msg._dataLArray[0]->setNumber(player->_PLAYER_ID);
		_msg._dataLArray[1]->setString(player->_KEY);
		_msg._dataLArray[2]->setNumber(player->_status);
		_msg._dataLArray[3]->setNumber(room->_PLAYERS_STATUS);

		_msg._dataLArray[4]->setNumber(player->_double);
		_msg._dataLArray[5]->setString(_EX_DATA.c_str());
		_msg._dataLArray[6]->setNumber(_lastAround?1:0);

		room->brodcast<MSG_S2C_ALL_ADD_DOUBLE>(_msg, player);
		SEND_MSG<MSG_S2C_ALL_ADD_DOUBLE>(_msg, client);

		//若是最后一局，通知结束以后，房间解散
		if( _lastAround )
		{
			GameRooms::Instance()->disbandRoomAfterGameOver(room->_ROOM_ID);
		}
	}
	else
	{
		MSG_S2C_ALL_ADD_DOUBLE _msg;
		_msg._error_code = PROTOCAL_ERROR_GAME;
		_msg._error_ex   = _hr;
		SEND_MSG<MSG_S2C_ALL_ADD_DOUBLE>(_msg, client);
	}

	return true;
}

NET_CALLBACK(C2S_NEXT_AROUND)
{
	//////////////////////////////////////////////////////////////////////////
	CHECK_MSG_PARAM(MSG_C2S_NEXT_AROUND);
	CHECK_MSG_LOGIN(client);
	GET_PLAYER;
	GET_ROOM;
	CHECK_ROOM_STATUS(EPS_DEALER);

	//////////////////////////////////////////////////////////////////////////
	ENUM_GAME_STATUS_ERROR _hr = player->endThisAround();

	if( _hr == EGSE_OK )
	{
		//////////////////////////////////////////////////////////////////////////
		REFRESH_ROOM_GAMESTATUS(room);
		std::string _EX_DATA = "0";

		bool _checkEnd = false;

		if( room->_PLAYERS_STATUS == EPS_END )
		{
			room->_PLAYERS_STATUS = EPS_NONE;

			PLAYER_LIST _playerList;
			room->getPlayersInRoom(_playerList);

			for( PLAYER_LIST::iterator cell = _playerList.begin(); cell != _playerList.end(); cell++ )
			{
				BASE_PLAYER* _playerData = *cell;
				_playerData->resetData();
			}

			room->getPlayersInfo(_EX_DATA);

			_checkEnd = true;
		}

		MSG_S2C_NEXT_AROUND _msg;
		_msg._dataLArray[0]->setNumber(player->_PLAYER_ID);
		_msg._dataLArray[1]->setString(player->_KEY);
		_msg._dataLArray[2]->setNumber(player->_status);
		_msg._dataLArray[3]->setNumber(room->_PLAYERS_STATUS);
		_msg._dataLArray[4]->setNumber(EPS_DEALER);
		_msg._dataLArray[5]->setString(_EX_DATA.c_str());

		if( _checkEnd )
		{
			_msg._dataLArray[4]->setNumber(EPS_NONE);
			room->brodcast<MSG_S2C_NEXT_AROUND>(_msg, player);
		}

		SEND_MSG<MSG_S2C_NEXT_AROUND>(_msg, client);

	}
	else
	{
		MSG_S2C_NEXT_AROUND _msg;
		_msg._error_code = PROTOCAL_ERROR_GAME;
		_msg._error_ex   = _hr;
		SEND_MSG<MSG_S2C_NEXT_AROUND>(_msg, client);
	}

	return true;
}

NET_CALLBACK(C2S_APPLICATE_LEAVE)
{
	//////////////////////////////////////////////////////////////////////////
	CHECK_MSG_PARAM(MSG_C2S_APPLICATE_LEAVE);
	CHECK_MSG_LOGIN(client);
	GET_PLAYER;
	GET_ROOM;
	//CHECK_ROOM_STATUS(EPS_DEALER);

	const int _agree = data[JSON_FLAG].asInt();

	switch (_agree)
	{
	case 0:
		{
			//////////////////////////////////////////////////////////////////////////
			PLAYER_LIST _player_list;
			room->getPlayersInRoom(_player_list);

			for( PLAYER_LIST::iterator cell = _player_list.begin(); cell != _player_list.end(); cell++ )
			{
				BASE_PLAYER* _player = *cell;
				_player->_leaveStatus = ELS_NONE;
			}

			//////////////////////////////////////////////////////////////////////////
			Json::Value _root;
			std::string _EX_DATA;

			_root[JSON_PLAYER_UID] = player->_PLAYER_ID;
			_root[JSON_PLAYER_LEAVE_STATUS] = ELS_REFUSE;

			Json::FastWriter _writer;
			_EX_DATA = _writer.write(_root);

			//////////////////////////////////////////////////////////////////////////
			room->_AgreeDisbandStatus = 0;

			//////////////////////////////////////////////////////////////////////////
			MSG_S2C_ALL_APPLICATE_LEAVE _msg;
			_msg._dataLArray[0]->setNumber(player->_PLAYER_ID);
			_msg._dataLArray[1]->setString(_EX_DATA.c_str());
			_msg._dataLArray[2]->setNumber(ENUM_LEAVE_RESULT::ELR_NOTPASS);

			SEND_MSG<MSG_S2C_ALL_APPLICATE_LEAVE>(_msg, client);
			room->brodcast<MSG_S2C_ALL_APPLICATE_LEAVE>(_msg, player);

			break;
		}
	case 1:
		{
			//////////////////////////////////////////////////////////////////////////
			ENUM_GAME_STATUS_ERROR _hr = player->applicateLeave();

			if( _hr == EGSE_OK )
			{
				std::string _EX_DATA;
				const int _count = room->getExDataFromApplicateLeave(_EX_DATA);

				//////////////////////////////////////////////////////////////////////////
				MSG_S2C_ALL_APPLICATE_LEAVE _msg;
				_msg._dataLArray[0]->setNumber(player->_PLAYER_ID);
				_msg._dataLArray[1]->setString(_EX_DATA.c_str());
				_msg._dataLArray[2]->setNumber(ENUM_LEAVE_RESULT::ELR_NONE);

				//////////////////////////////////////////////////////////////////////////
				if( _count == 1 )
				{
					room->_agree_disband = time(NULL);
					_msg._dataLArray[2]->setNumber(ENUM_LEAVE_RESULT::ELR_APPLICATE);

					SEND_MSG<MSG_S2C_ALL_APPLICATE_LEAVE>(_msg, client);
					room->brodcast<MSG_S2C_ALL_APPLICATE_LEAVE>(_msg, player);
					room->_AgreeDisbandStatus = 1;
				}
				else if( _count == room->getCurrentPlayersCount() )
				{
					//直接退出
					_msg._dataLArray[2]->setNumber(ENUM_LEAVE_RESULT::ELR_ALL_AGREE);

					SEND_MSG<MSG_S2C_ALL_APPLICATE_LEAVE>(_msg, client);
					room->brodcast<MSG_S2C_ALL_APPLICATE_LEAVE>(_msg, player);

					GameRooms::Instance()->disbandRoomAfterGameOver(room->_ROOM_ID);
				}
				else
				{
					SEND_MSG<MSG_S2C_ALL_APPLICATE_LEAVE>(_msg, client);
					room->brodcast<MSG_S2C_ALL_APPLICATE_LEAVE>(_msg, player);
				}
			}
			else
			{
				MSG_S2C_ALL_APPLICATE_LEAVE _msg;
				_msg._error_code = PROTOCAL_ERROR_GAME;
				_msg._error_ex   = _hr;
				SEND_MSG<MSG_S2C_ALL_APPLICATE_LEAVE>(_msg, client);
			}

			break;
		}
	}


	return true;
}
