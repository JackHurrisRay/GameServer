#include "GameDouNiu.h"
#include "game_common.h"
#include "GameRoom.h"
#include "json/json.h"

#include "./../game/message.h"
#include "./../core/JackBase64.h"

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

//炸弹五小牛
extern bool check_zhadan(BASE_POKE_CARD** _cardArray, BASE_POKE_CARD*& _flagCard);
extern bool check_wuxiaoniu(BASE_POKE_CARD** _cardArray, BASE_POKE_CARD*& _flagCard);
bool check_zhadanwuxiao(BASE_POKE_CARD** _cardArray, BASE_POKE_CARD*& _flagCard)
{
	bool check = false;

	BASE_POKE_CARD* _card[2];

	if( check_zhadan(_cardArray, _card[0]) && check_wuxiaoniu(_cardArray, _card[1]) )
	{
		_flagCard = _card[0];
		check = true;
	}

	return check;
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
	_status        = EPS_NONE;
	_leaveStatus   = ELS_NONE;
	_winType       = EWCT_COUNT;
	_winCard       = NULL;
	_currentScore  = 0;
	_zhuang = -1;
	_double = -1;
	_isZhuang = false;
	
	memset(_card, 0, sizeof(BASE_POKE_CARD) * MAX_CARD_PER_PLAYER);
}

ENUM_WIN_CARD_TYPE GAME_PLAYER_DATA::process_WinCard(BASE_POKE_CARD*& _flagCard)
{
	ENUM_WIN_CARD_TYPE _type = EWCT_NONE;

	FUNC_CHECK_NIU* _func[] =
	{
		&check_zhadanwuxiao,
		&check_zhadan,
		&check_wuxiaoniu,
		&check_wuhuaniu,
		&check_sihuaniu
	};

	const int _FIRST_WIN_TYPE[] =
	{
		EWCT_ZHANDANWUXIAO,
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
void GAME_PLAYER_DATA::checkWinCard()
{
	_winType = process_WinCard(_winCard);
}

void GAME_PLAYER_DATA::getPokeCard(INTEGER_ARRAY& _pokecard_data)
{
	_pokecard_data.clear();

	for( int i=0; i<MAX_CARD_PER_PLAYER; i++ )
	{
		BASE_POKE_CARD* _pCard = _card[i];

		if( _pCard != NULL )
		{
			_pokecard_data.push_back(_pCard->_guid);
		}
	}
}

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

	if( _doubleValue < 0 || _doubleValue > 255 )
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

ENUM_GAME_STATUS_ERROR GAME_PLAYER_DATA::endThisAround()
{
	ENUM_GAME_STATUS_ERROR _hr = EGSE_UNKNOWN;

	if( _status == EPS_DEALER && _leaveStatus == ELS_NONE )
	{
		_status = EPS_END;
		_hr     = EGSE_OK;
	}
	else
	{
		_hr = EGSE_CURRENT_STATUS_ERROR;
	}

	return _hr;
}

ENUM_GAME_STATUS_ERROR GAME_PLAYER_DATA::applicateLeave()
{
	ENUM_GAME_STATUS_ERROR _hr = EGSE_UNKNOWN;

	if( _leaveStatus == ELS_NONE )
	{
		_leaveStatus = ELS_AGREE;
		_hr          = EGSE_OK;
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

bool GAME_DOU_NIU::getPlayerPokeCardInfo(BASE_ROOM* _room, std::string& _info)
{
	bool _check = false;

	Json::Value _root;

	PLAYER_LIST _playerList;
	_room->getPlayersInRoom(_playerList);

	if( _playerList.size() > 1 )
	{
		/*
		PLAYER_LIST::iterator cell;

		for( cell = _playerList.begin(); cell != _playerList.end(); cell++ )
		{
			BASE_PLAYER* _player = *cell;

			Json::Value _playerJsonData;

			_playerJsonData[JSON_PLAYER_UID] = _player->_PLAYER_ID;
			_playerJsonData[JSON_PLAYER_KEY] = _player->_KEY;
			_playerJsonData[JSON_ZHUANG]     = _player->_isZhuang?1:0;
			_playerJsonData[JSON_DOUBLE]     = _player->_double;

			for( int i=0; i<_limit; i++ )
			{
				_playerJsonData[JSON_POKECARD].append( _player->_card[i]->_guid );
			}

			_root[JSON_PLAYER].append(_playerJsonData);
		}
		*/

		/*
		//////////////////////////////////////////////////////////////////////////
		Json::Value _playerJsonData;

		_playerJsonData[JSON_PLAYER_UID] = _player->_PLAYER_ID;
		_playerJsonData[JSON_PLAYER_KEY] = _player->_KEY;
		_playerJsonData[JSON_ZHUANG]     = _player->_isZhuang?1:0;
		_playerJsonData[JSON_DOUBLE]     = _player->_double;

		for( int i=0; i<MAX_CARD_PER_PLAYER; i++ )
		{
			_playerJsonData[JSON_POKECARD].append( _player->_card[i]->_guid );
		}

		_root[JSON_PLAYER].append(_playerJsonData);
		*/

		//////////////////////////////////////////////////////////////////////////
		_root[JSON_ZHUANG_VALUE] = _room->_zhuangPlayer->_zhuang;
		_root[JSON_PLAYER_UID]   = _room->_zhuangPlayer->_PLAYER_ID;

		Json::FastWriter _writer;
		_info = _writer.write(_root);

		_check = true;
	}

	return _check;
}

int GAME_DOU_NIU::getWinTypeScore(BASE_PLAYER* _player)
{
	int _value = 0;
	const int _winTypeIndex = _player->_winType;
	_value = JackBase64::GAME_CONFIG::Instance()->_SCORE_FOR_WINTYPE[_winTypeIndex];
	return _value;
}

bool compareWinPlayer( const BASE_PLAYER* player1, const BASE_PLAYER* player2 )
{
	if( player1->_winType > player2->_winType )
	{
		return true;
	}
	else if( player1->_winType == player2->_winType && player1->_winType != EWCT_NONE )
	{
		BASE_POKE_CARD* _card[2] = 
			{
				player1->_winCard,
				player2->_winCard
		    };

		if( _card[0]->_value > _card[1]->_value )
		{
			return true;
		}
		else if( _card[0]->_value == _card[1]->_value )
		{
			return _card[0]->_suit < _card[1]->_suit;
		}
		else
		{
			return false;
		}
	}
	else if( player1->_winType == EWCT_NONE && player2->_winType == EWCT_NONE )
	{
		return player1->_isZhuang > player2->_isZhuang;
	}
	else
	{
		return false;
	}

}

void GAME_DOU_NIU::computerPlayerScore(BASE_ROOM* room, std::string& _info)
{
	//////////////////////////////////////////////////////////////////////////
	PLAYER_LIST _playerList;
	room->getPlayersInRoom(_playerList);

	BASE_PLAYER* _zhuangPlayer = room->_zhuangPlayer;

	//////////////////////////////////////////////////////////////////////////
	//排序
	_playerList.sort(compareWinPlayer);

	const int _baseScore  = room->_baseScore;                               //底分
	const int _baseDouble = room->_zhuangValue;                             //初始倍数
	const int  _zhuangDouble = _zhuangPlayer->_double;                      //庄倍数

	//////////////////////////////////////////////////////////////////////////
	std::vector<BASE_PLAYER*> PLAYER_ARRAY;
	int _arrayIndex      = 0;
	int _zhuangIndexTemp = -1;

	for( PLAYER_LIST::iterator cell = _playerList.begin(); cell != _playerList.end(); cell++ )
	{
		//////////////////////////////////////////////////////////////////////////
		BASE_PLAYER* _player = *cell;

		PLAYER_ARRAY.push_back(_player);
		if( _player == _zhuangPlayer )
		{
			_zhuangIndexTemp = _arrayIndex;
		}

		_arrayIndex += 1;
	}

	//////////////////////////////////////////////////////////////////////////
	const int _playerCount = PLAYER_ARRAY.size();
	const int _zhuangIndex = _zhuangIndexTemp;

	
	//////////////////////////////////////////////////////////////////////////
	//算出庄赢的分数
	int    _zhuangWinScore    = 0;
	int    _zhuangtotalDouble = 0;

	{
		BASE_PLAYER* _player   = _zhuangPlayer;

		_zhuangtotalDouble = _zhuangDouble + _baseDouble + GAME_DOU_NIU::getWinTypeScore(_player);		
		_zhuangWinScore        = _baseScore * _zhuangtotalDouble;
	}

	long long _zhuangCurrentScore = 0;
	for( int i=0; i<_playerCount; i++ )
	{
		BASE_PLAYER* _player = PLAYER_ARRAY[i];

		if( i < _zhuangIndex )
		{
			//赢庄
			const int _totalDouble = _player->_double + _baseDouble + GAME_DOU_NIU::getWinTypeScore(_player) + _zhuangDouble;
			_player->_currentScore = _baseScore * _totalDouble;

			////
			_zhuangCurrentScore -= _player->_currentScore;
		}
		else if( i > _zhuangIndex )
		{
			//输庄
			_player->_currentScore = -_baseScore * ( _player->_double + _zhuangtotalDouble );

			////
			_zhuangCurrentScore -= _player->_currentScore;
		}
	}

	_zhuangPlayer->_currentScore = _zhuangCurrentScore;

	for( int i=0; i<_playerCount; i++ )
	{
		BASE_PLAYER* _player = PLAYER_ARRAY[i];
		_player->_totalSCORE += _player->_currentScore;
	}

	//////////////////////////////////////////////////////////////////////////
	Json::Value _root;
	Json::Value _zhuangInfo;

	//////////////////////////////////////////////////////////////////////////
	_zhuangInfo[JSON_PLAYER_UID]              = _zhuangPlayer->_PLAYER_ID;
	_zhuangInfo[JSON_PLAYER_KEY]              = _zhuangPlayer->_KEY;
	_zhuangInfo[JSON_BASESCORE]               = _baseScore;
	_zhuangInfo[JSON_BASEDOUBLE]              = _baseDouble;
	_zhuangInfo[JSON_PLAYER_CURRENTSCORE]     = _zhuangPlayer->_currentScore;
	_zhuangInfo[JSON_PLAYER_TOTALSCORE]       = _zhuangPlayer->_totalSCORE;
	_zhuangInfo[JSON_POKECARD_WINTYPE]        = _zhuangPlayer->_winType;

	_root[JSON_PLAYER_ZHUANG]                 = _zhuangInfo;

	//////////////////////////////////////////////////////////////////////////
	for( int i=0; i<_playerCount; i++ )
	{
		BASE_PLAYER* _player = PLAYER_ARRAY[i];
		Json::Value _playerInfo;

		_playerInfo[JSON_PLAYER_UID] = _player->_PLAYER_ID;
		_playerInfo[JSON_PLAYER_KEY] = _player->_KEY;

		_playerInfo[JSON_DOUBLE]     = _player->_double;

		_playerInfo[JSON_PLAYER_CURRENTSCORE]     = _player->_currentScore;
		_playerInfo[JSON_PLAYER_TOTALSCORE]       = _player->_totalSCORE;
		_playerInfo[JSON_POKECARD_WINTYPE]        = _player->_winType;

		for( int i=0; i<MAX_CARD_PER_PLAYER; i++ )
		{
			BASE_POKE_CARD* _card = _player->_card[i];

			if( _card )
			{
				_playerInfo[JSON_POKECARD].append(_card->_guid);
			}
		}

		_root[JSON_PLAYER].append(_playerInfo);
	}

	Json::FastWriter _writer;
	_info = _writer.write(_root);


}





