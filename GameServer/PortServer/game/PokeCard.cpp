#include "PokeCard.h"
#include "./../core/utilcore.h"

//////////////////////////////////////////////////////////////////////////
BASE_POKE_CARD::BASE_POKE_CARD(unsigned char __guid):
	_guid(__guid),
	_value((unsigned char)((__guid / 4) + 1)),
	_suit(__guid % 4),
	_score((_value > 10)?10:_value)
{

}

//////////////////////////////////////////////////////////////////////////
CPokeCard::CPokeCard(void)
{
	for( int i=0; i<MAX_CARD_COUNT; i++ )
	{
		m_card[i] = new BASE_POKE_CARD(i);
	}
}


CPokeCard::~CPokeCard(void)
{
	for( int i=0; i<MAX_CARD_COUNT; i++ )
	{
		delete m_card[i];
		m_card[i] = 0;
	}
}

const LP_BASE_POKE_CARD* CPokeCard::get_card_array()
{
	return m_currentCard;
};

void CPokeCard::get_card_list(POKE_LIST& cardlist)
{
	for( int i=0; i<MAX_CARD_COUNT; i++ )
	{
		cardlist.push_back(m_currentCard[i]);
	}
}

void CPokeCard::Shuffle_cards()
{
	POKE_LIST _pokeList;
	for( int i=0; i<MAX_CARD_COUNT; i++ )
	{
		_pokeList.push_back(m_card[i]);
	}

	m_cardPool.clear();
	while ( _pokeList.size() > 0 )
	{
		const int _current_size   = _pokeList.size();
		const int _index = GET_RAND() % _current_size;
		
		POKE_LIST::iterator _cell = _pokeList.begin();

		for (int i = 0; i < _index; i++)
		{
			_cell++;
		}

		if( _cell != _pokeList.end() )
		{
			BASE_POKE_CARD* _card = *_cell;
			if( _card != NULL )
			{
				_pokeList.remove(_card);
				m_cardPool.push_back(_card);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	int _index = 0;
	POKE_LIST::iterator cell;
	for( cell = m_cardPool.begin(); cell != m_cardPool.end(); cell++ )
	{
		m_currentCard[_index++] = *cell;
	}

	for( int i=0; i<6; i++ )
	{
		reShuffle_cards();
	}
}

void CPokeCard::reShuffle_cards()
{
	POKE_LIST _pokeList;
	for( int i=0; i<MAX_CARD_COUNT; i++ )
	{
		_pokeList.push_back(m_currentCard[i]);
	}

	m_cardPool.clear();
	while ( _pokeList.size() > 0 )
	{
		const int _current_size   = _pokeList.size();
		const int _index = GET_RAND() % _current_size;

		POKE_LIST::iterator _cell = _pokeList.begin();

		for (int i = 0; i < _index; i++)
		{
			_cell++;
		}

		if( _cell != _pokeList.end() )
		{
			BASE_POKE_CARD* _card = *_cell;
			if( _card != NULL )
			{
				_pokeList.remove(_card);
				m_cardPool.push_back(_card);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	int _index = 0;
	POKE_LIST::iterator cell;
	for( cell = m_cardPool.begin(); cell != m_cardPool.end(); cell++ )
	{
		m_currentCard[_index++] = *cell;
	}
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
const POKE_IN_HAND POKE_WIN_CARD_WUXIAOZHADAN[] =
{
	//////////////////////////////////////////////////////////////////////////
	//炸弹五小牛
	{0,1,2,3,4},
	{0,1,2,3,5},
	{0,1,2,3,6},
	{0,1,2,3,7},

	{0,1,2,3,8},
	{0,1,2,3,9},
	{0,1,2,3,10},
	{0,1,2,3,11},

	{0,1,2,3,12},
	{0,1,2,3,13},
	{0,1,2,3,14},
	{0,1,2,3,15},

	{0,1,2,3,16},
	{0,1,2,3,17},
	{0,1,2,3,18},
	{0,1,2,3,19},

	{0,1,2,3,20},
	{0,1,2,3,21},
	{0,1,2,3,22},
	{0,1,2,3,23},
};

const POKE_IN_HAND POKE_WIN_CARD_ZHADAN[] = 
{
	//炸弹牛
	{0,1,2,3,-1},
	{4,5,6,7,-1},
	{8,9,10,11,-1},
	{12,13,14,15,-1},
	{16,17,18,19,-1},
	{20,21,22,23,-1},
	{24,25,26,27,-1},
	{28,29,30,31,-1},
	{32,33,34,35,-1},
	{36,37,38,39,-1},
	{40,41,42,43,-1},
	{44,45,46,47,-1},
	{48,49,50,51,-1},
};

//////////////////////////////////////////////////////////////////////////
int checkRepeat( const int _scoreTemp[5] )
{
	int _reuslt = 0;

	std::list<int> _scoreList;

	for( int i=0; i<5; i++ )
	{
		if( std::find(_scoreList.begin(), _scoreList.end(), _scoreTemp[i]) == _scoreList.end() )
		{
			_scoreList.push_back(_scoreTemp[i]);
		}
	}

	_reuslt = _scoreList.size();

	return _reuslt;
}

//////////////////////////////////////////////////////////////////////////
int removeByCardIndex( int _index, POKE_LIST& _pokeList )
{
	int _result = -1;
	LP_BASE_POKE_CARD _findCard = NULL;

	for( POKE_LIST::iterator cell = _pokeList.begin(); cell != _pokeList.end(); cell++ )
	{
		LP_BASE_POKE_CARD _card = *cell;

		if( _index == -1 )
		{
			_findCard = _card;
			_result = _card->_guid;

			break;
		}
		else if( _card->_guid == _index )
		{
			_findCard = _card;
			_result = _card->_guid;

			break;
		}
	}

	if( _findCard )
	{
		_pokeList.remove(_findCard);
	}

	return _result;
}

int removeByCardScore( int _score, POKE_LIST& _pokeList )
{
	int _result = -1;
	LP_BASE_POKE_CARD _findCard = NULL;

	for( POKE_LIST::iterator cell = _pokeList.begin(); cell != _pokeList.end(); cell++ )
	{
		LP_BASE_POKE_CARD _card = *cell;

		if( _card->_score == _score )
		{
			_findCard = _card;
			_result = _card->_guid;

			break;
		}
	}

	if( _findCard )
	{
		_pokeList.remove(_findCard);
	}

	return _result;
}

//该方法求出五小炸弹和炸弹
const int* takePokeCardPrepare(int _cardIndex[5], POKE_LIST& _pokeList)
{
	int _result[5];

	for( int i=0; i<5; i++ )
	{
		_result[i] =
		removeByCardIndex(_cardIndex[i], _pokeList);
	}

	return _result;
}

const int* takePokeCardByScore(int _score[5], POKE_LIST& _pokeList)
{
	int _result[5];

	for( int i=0; i<5; i++ )
	{
		_result[i] =
		removeByCardScore(_score[i], _pokeList);
	}

	return _result;
}

//求出五小牛
const int* getWuxiaoCardPrepare(POKE_LIST& _pokeList)
{
	int _score[5];

	for(;;)
	{
		const int _scoreTemp[5] = {GET_RAND()%5, GET_RAND()%5, GET_RAND()%5, GET_RAND()%5, GET_RAND()%5};

		if( _scoreTemp[0] + _scoreTemp[1] + _scoreTemp[2] + _scoreTemp[3] + _scoreTemp[4] > 10 )
		{
			continue;
		}
		else if( checkRepeat(_scoreTemp) <= 2 )
		{
			continue;
		}
		else
		{
			memcpy(_score, _scoreTemp, sizeof(_score));
			break;
		}
	}

	const int* _result = takePokeCardByScore(_score, _pokeList);

	return _result;
}

//求出五花牛
const int* getWuhuaCardPrepare(POKE_LIST& _pokeList)
{
	int _result[5];
	int _index = 0;

	for( POKE_LIST::iterator cell = _pokeList.begin(); cell != _pokeList.end() && _index < 5; cell++ )
	{
		LP_BASE_POKE_CARD _card = *cell;

		if( _card->_score == 10 && _card->_guid != 36 && _card->_guid != 37 && _card->_guid != 38 && _card->_guid != 39 )
		{
			_result[_index++] = _card->_guid;
		}
	}

	for( int i=0; i<5; i++ )
	{
		removeByCardIndex(_result[i], _pokeList);
	}

	return _result;
}

//////////////////////////////////////////////////////////////////////////
void takeRandFlag(int _randflag[2])
{
	if( _randflag[0] == _randflag[1] )
	{
		if( _randflag[0] <= 2 )
		{
			_randflag[1] += 1;
		}
		else 
		{
			_randflag[1] -= 1;
		}
	}
}

int* takeRandIndex()
{
	int _result[] = {0,1,2,3,4};

	int _randFlag1[] = {GET_RAND()%5, GET_RAND()%5};
	int _randFlag2[] = {GET_RAND()%5, GET_RAND()%5};

	takeRandFlag(_randFlag1);
	takeRandFlag(_randFlag2);

	//交换1
	{
		int temp[] = {_result[_randFlag1[0]], _result[_randFlag1[1]]}; 
		_result[_randFlag1[0]] = temp[1];
		_result[_randFlag1[1]] = temp[0];
	}

	{
		int temp[] = {_result[_randFlag2[0]], _result[_randFlag2[1]]}; 
		_result[_randFlag2[0]] = temp[1];
		_result[_randFlag2[1]] = temp[0];
	}


	return _result;
}

//求出四花牛
const int* getSihuaCardPrepare(POKE_LIST& _pokeList)
{
	int _result[5];
	int _tempResult[5];
	int _index = 0;

	for( POKE_LIST::iterator cell = _pokeList.begin(); cell != _pokeList.end() && _index < 4; cell++ )
	{
		LP_BASE_POKE_CARD _card = *cell;

		if( _card->_score == 10 && _card->_guid != 36 && _card->_guid != 37 && _card->_guid != 38 && _card->_guid != 39 )
		{
			_tempResult[_index++] = _card->_guid;
		}
	}

	for( int i=0; i<5; i++ )
	{
		removeByCardIndex(_tempResult[i], _pokeList);
	}

	for( POKE_LIST::iterator cell = _pokeList.begin(); cell != _pokeList.end() && _index < 4; cell++ )
	{
		LP_BASE_POKE_CARD _card = *cell;

		if( _card->_guid == 36 || _card->_guid == 37 || _card->_guid == 38 || _card->_guid == 39 )
		{
			_tempResult[_index++] = _card->_guid;
			break;
		}
	}

	removeByCardIndex(_tempResult[4], _pokeList);

	//////////////////////////////////////////////////////////////////////////
	//rand for result
	const int* _randindex = takeRandIndex();

	for( int i=0; i<5; i++ )
	{
		const int _newIndex = _randindex[i];
		_result[i] = _tempResult[_newIndex];
	}

	return _result;
}

//求出有牛牌，3张相加为10
const int* getNiuCardStyle(POKE_LIST& _pokeList)
{
	int _result[3];
	int _index = 0;
	int _totalScore = 0;

	//先抽前两张
	for( POKE_LIST::iterator cell = _pokeList.begin(); cell != _pokeList.end() && _index < 3; cell++ )
	{
		LP_BASE_POKE_CARD _card = *cell;

		if( _index < 2 )
		{
			_result[_index++] = _card->_guid;
			_totalScore += _card->_score;
		}
		else
		{
			if( (_card->_score + _totalScore)%10 == 0 )
			{
				_result[_index++] = _card->_guid;
				break;
			}
		}
	}

	for( int i=0; i<3; i++ )
	{
		removeByCardIndex(_result[i], _pokeList);
	}

	return _result;
}

//求出牛牛
const int* getNiuniu(POKE_LIST& _pokeList)
{
	int _result[5];

	//////////////////////////////////////////////////////////////////////////
	const int* _top3Cards = getNiuCardStyle(_pokeList);

	for( int i=0; i<3; i++ )
	{
		_result[i] = _top3Cards[i];
	}

	//////////////////////////////////////////////////////////////////////////
	int _index = 3;
	int _totalScore = 0;
	for( POKE_LIST::iterator cell = _pokeList.begin(); cell != _pokeList.end() && _index < 5; cell++ )
	{
		LP_BASE_POKE_CARD _card = *cell;

		if( _index < 4 )
		{
			_result[_index++] = _card->_guid;
			_totalScore += _card->_score;
		}
		else
		{
			if( (_card->_score + _totalScore)%10 == 0 )
			{
				_result[_index++] = _card->_guid;
				break;
			}
		}
	}

	for( int i=3; i<5; i++ )
	{
		removeByCardIndex(_result[i], _pokeList);
	}

	return _result;
}

//求牛1~牛9，牛牛为0
const int* getNiuPrepare(int _niuValue, POKE_LIST& _pokeList)
{
	int _result[5];

	//////////////////////////////////////////////////////////////////////////
	const int* _top3Cards = getNiuCardStyle(_pokeList);

	for( int i=0; i<3; i++ )
	{
		_result[i] = _top3Cards[i];
	}

	//////////////////////////////////////////////////////////////////////////
	int _index = 3;
	int _totalScore = 0;
	for( POKE_LIST::iterator cell = _pokeList.begin(); cell != _pokeList.end() && _index < 5; cell++ )
	{
		LP_BASE_POKE_CARD _card = *cell;

		if( _index < 4 )
		{
			_result[_index++] = _card->_guid;
			_totalScore += _card->_score;
		}
		else
		{
			if( (_card->_score + _totalScore)%10 == _niuValue )
			{
				_result[_index++] = _card->_guid;
				break;
			}
		}
	}

	for( int i=3; i<5; i++ )
	{
		removeByCardIndex(_result[i], _pokeList);
	}

	return _result;
}