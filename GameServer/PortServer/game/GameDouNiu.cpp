#include "GameDouNiu.h"
#include "game_common.h"
#include "GameRoom.h"
#include "json/json.h"

#include "./../game/message.h"

//////////////////////////////////////////////////////////////////////////
ALLOC_POKECARD GAME_DOU_NIU::_ALLOC_POKECARD(MAX_ROOM_LIMIT, "ALLOC POKECARD");

//////////////////////////////////////////////////////////////////////////
BASE_POKE_CARD* get_card_of_max_value(BASE_POKE_CARD** _cardArray, int _count = 5)
{
	BASE_POKE_CARD* _card = _cardArray[0];

	for( int i=1; i<_count; i++ )
	{
		if( _card->_value < _cardArray[i]->_value )
		{
			_card = _cardArray[i];
		}
		else if( _card->_value == _cardArray[i]->_value )
		{
			if( _card->_suit > _cardArray[i]->_suit )
			{
				_card = _cardArray[i];
			}
		}
	}

	return _card;
}

//五小牛
bool check_wuxiaoniu(BASE_POKE_CARD** _cardArray, BASE_POKE_CARD*& _flagCard)
{
	bool check = false;

	if( (_cardArray[0]->_score + _cardArray[1]->_score + _cardArray[2]->_score + _cardArray[3]->_score + _cardArray[4]->_score) < 10 )
	{
		_flagCard = get_card_of_max_value(_cardArray);
		check = true;
	}

	return check;
}

//炸弹
bool check_zhadan(BASE_POKE_CARD** _cardArray, BASE_POKE_CARD*& _flagCard)
{
	bool check = false;

	std::list<int> _VALUE_LIST;

	int _zhadan_value = -1;
	for( int i=0; i<5; i++ )
	{
		int _value = _cardArray[i]->_value;

		if( std::find(_VALUE_LIST.begin(), _VALUE_LIST.end(), _value) == _VALUE_LIST.end() )
		{
			_VALUE_LIST.push_back(_value);
		}
		else
		{
			_zhadan_value = _value;
		}
	}

	if( _VALUE_LIST.size() == 2 )
	{
		check = true;
	}

	if( check )
	{
		BASE_POKE_CARD* _zhadan[4];
		int _zhadan_index = 0;

		for( int i = 0; i<5; i++ )
		{
			if( _cardArray[i]->_value == _zhadan_value )
			{
				_zhadan[_zhadan_index++] = _cardArray[i];
			}
		}

		check = _zhadan_index == 4;

		if( check )
		{
			_flagCard = get_card_of_max_value(_zhadan, 4);
		}
	}

	return check;
}

//五花牛
bool check_wuhuaniu(BASE_POKE_CARD** _cardArray, BASE_POKE_CARD*& _flagCard)
{
	bool check = false;

	if( _cardArray[0]->_value > 10 && 
		_cardArray[1]->_value > 10 && 
		_cardArray[2]->_value > 10 && 
		_cardArray[3]->_value > 10 && 
		_cardArray[4]->_value > 10 
		)
	{
		_flagCard = get_card_of_max_value(_cardArray);
		check = true;
	}

	return check;
}

//四花牛
bool check_sihuaniu(BASE_POKE_CARD** _cardArray, BASE_POKE_CARD*& _flagCard)
{
	bool check = false;

	int _tempCount1 = 0;
	int _tempCount2 = 0;

	for( int i=0; i<5; i++ )
	{
		BASE_POKE_CARD* _card = _cardArray[i];

		if( _card->_value > 10 )
		{
			_tempCount1 += 1;
		}
		else if( _card->_value == 10 )
		{
			_tempCount2 += 1;
		}
	}

	check = (_tempCount1 == 4 && _tempCount2 == 1);
	if( check )
	{
		_flagCard = get_card_of_max_value(_cardArray);
	}

	return check;
}

//选出有牛
bool check_niu(BASE_POKE_CARD** _cardArray, BASE_POKE_CARD* _lastCard[2])
{
	bool check = false;

	struct CARD_NIU
	{
		const int _card[3];
		const int _last[2];

		bool _checkNiu(BASE_POKE_CARD** _cardArray)
		{
			int _result =
				_cardArray[ _card[0] ]->_score + 
				_cardArray[ _card[1] ]->_score + 
				_cardArray[ _card[2] ]->_score;

			return (_result % 10 == 0);
		}
	};
	
	CARD_NIU _CARD_INDEX[] =
	{
		{0,1,2,  3,4},
		{0,1,3,  2,4},
		{0,1,4,  2,3},
		{0,2,3,  1,4},
		{0,2,4,  1,3},
		{0,3,4,  1,2},
		{1,2,3,  0,4},
		{1,2,4,  0,3},
		{1,3,4,  0,2},
		{2,3,4,  0,1}
	};

	CARD_NIU* _card_niu = NULL;
	for( int i=0; i<(sizeof(_CARD_INDEX) / sizeof(CARD_NIU)); i++ )
	{
		//////////////////////////////////////////////////////////////////////////
		if( _CARD_INDEX[i]._checkNiu(_cardArray) )
		{
			_card_niu = &_CARD_INDEX[i];
			break;
		}
	}

	if( _card_niu != NULL )
	{
		_lastCard[0] = _cardArray[ _card_niu->_last[0] ];
		_lastCard[1] = _cardArray[ _card_niu->_last[1] ];
		check = true;
	}

	return check;
}

typedef bool FUNC_CHECK_NIU(BASE_POKE_CARD** _cardArray, BASE_POKE_CARD*& _flagCard);

//////////////////////////////////////////////////////////////////////////
void GAME_PLAYER_DATA::resetData()
{
	_status = EPS_NONE;
	_SCORE  = 0;
	_zhuang = 0;
	_double = 0;
	
	memset(_card, 0, sizeof(BASE_POKE_CARD) * MAX_CARD_PER_PLAYER);
}

ENUM_WIN_CARD_TYPE GAME_PLAYER_DATA::process_WinCard(BASE_POKE_CARD*& _flagCard)
{
	ENUM_WIN_CARD_TYPE _type = EWCT_NONE;

	FUNC_CHECK_NIU* _func[] =
	{
		&check_zhadan,
		&check_wuxiaoniu,
		&check_wuhuaniu,
		&check_sihuaniu
	};

	const int _FIRST_WIN_TYPE[] =
	{
		EWCT_ZHADAN,
		EWCT_WUXIAONIU,
		EWCT_WUHUANIU,
		EWCT_SIHUANIU
	};

	for(int i=0; i<sizeof(_func)/sizeof(FUNC_CHECK_NIU*); i++)
	{
		if( (*_func)((BASE_POKE_CARD**)_card, _flagCard) )
		{
			_type = (ENUM_WIN_CARD_TYPE)_FIRST_WIN_TYPE[i];
			break;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	if( _type == EWCT_NONE )
	{
		BASE_POKE_CARD* _lastcard[2];
		if(check_niu(_card, _lastcard))
		{
			//////////////////////////////////////////////////////////////////////////
			const int _lastNiuValue = _lastcard[0]->_score + _lastcard[1]->_score;
			if( _lastNiuValue % 10 == 0 )
			{
				_type = EWCT_NIUNIU;
			} 
			else
			{
				int _niuResult = _lastNiuValue % 10;
				_type = (ENUM_WIN_CARD_TYPE)(EWCT_NONE + _niuResult);
			}

			_flagCard = get_card_of_max_value(_card);
		}
	}

	return _type;
}

//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/*                                                                      */
/************************************************************************/
ENUM_GAME_STATUS_ERROR GAME_PLAYER_DATA::readyGame()
{
	ENUM_GAME_STATUS_ERROR _hr = EGSE_UNKNOWN;

	if( _status == EPS_NONE )
	{
		_status = EPS_READY;
		_hr     = EGSE_OK;
		_zhuang = 0;
	}
	else
	{
		_hr = EGSE_CURRENT_STATUS_ERROR;
	}

	return _hr;
}

ENUM_GAME_STATUS_ERROR GAME_PLAYER_DATA::cancelReady()
{
	ENUM_GAME_STATUS_ERROR _hr = EGSE_UNKNOWN;

	if( _status == EPS_READY )
	{
		_status = EPS_NONE;
		_hr = EGSE_OK;
	}
	else
	{
		_hr = EGSE_CURRENT_STATUS_ERROR;
	}

	return _hr;
}

ENUM_GAME_STATUS_ERROR GAME_PLAYER_DATA::fightForZhuang(int _value)
{
	ENUM_GAME_STATUS_ERROR _hr = EGSE_UNKNOWN;

	if( _value <= 0 || _value > 255 )
	{
		return EGSE_DATA_ERROR;
	}

	if( _status == EPS_READY )
	{
		_zhuang = _value;
		_status = EPS_FIGHT_FOR_ZHUANG;
		_hr     = EGSE_OK;
	}
	else
	{
		_hr = EGSE_CURRENT_STATUS_ERROR;
	}

	return _hr;
}

ENUM_GAME_STATUS_ERROR GAME_PLAYER_DATA::doubleScore(int _doubleValue)
{
	ENUM_GAME_STATUS_ERROR _hr = EGSE_UNKNOWN;

	if( _doubleValue <= 0 || _doubleValue > 255 )
	{
		return EGSE_DATA_ERROR;
	}

	if( _status == EPS_FIGHT_FOR_ZHUANG )
	{
		_double = _doubleValue;
		_status = EPS_DEALER;
		_hr     = EGSE_OK;
	}
	else
	{
		_hr = EGSE_CURRENT_STATUS_ERROR;
	}

	return _hr;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
GAME_DOU_NIU::GAME_DOU_NIU():
	_PokeCard(_ALLOC_POKECARD.createData())
{

}

bool GAME_DOU_NIU::getPlayerPokeCardInfo(BASE_ROOM* _room, std::string& _info, int _limit)
{
	if( _limit >= MAX_CARD_PER_PLAYER )
	{
		return false;
	}

	bool _check = false;

	Json::Value _root;

	PLAYER_LIST _playerList;
	_room->getPlayersInRoom(_playerList);

	if( _playerList.size() > 1 )
	{
		PLAYER_LIST::iterator cell;

		for( cell = _playerList.begin(); cell != _playerList.end(); cell++ )
		{
			BASE_PLAYER* _player = *cell;

			Json::Value _playerJsonData;

			_playerJsonData[JSON_PLAYER_UID] = _player->_PLAYER_ID;
			_playerJsonData[JSON_PLAYER_KEY] = _player->_KEY;
			_playerJsonData[JSON_ZHUANG]     = ( _room->_zhuangPlayer == _player )?1:0;
			_playerJsonData[JSON_DOUBLE]     = _player->_double;

			for( int i=0; i<_limit; i++ )
			{
				_playerJsonData[JSON_POKECARD].append( _player->_card[i]->_guid );
			}

			_root[JSON_PLAYER].append(_playerJsonData);
		}

		_root[JSON_ZHUANG_VALUE] = _room->_zhuangPlayer->_zhuang;

		Json::FastWriter _writer;
		_info = _writer.write(_root);

		_check = true;
	}

	return _check;
}