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

void CPokeCard::Shuffle_cards()
{
	POKE_LIST _pokeList;
	for( int i=0; i<MAX_CARD_COUNT; i++ )
	{
		_pokeList.push_back(m_card[i]);
	}

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

}
