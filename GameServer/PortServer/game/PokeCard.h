#ifndef _POKECARD_H
#define _POKECARD_H
#include <list>
#include "./../core/MemAllocPool.h"

struct POKE_IN_HAND
{
	int cardIndex[5];
};

extern const POKE_IN_HAND POKE_WIN_CARD_WUXIAOZHADAN[];
extern const POKE_IN_HAND POKE_WIN_CARD_ZHADAN[];

struct BASE_POKE_CARD
{
	const unsigned char _guid;  //1~10 J,Q,K
	const unsigned char _value; //J:11 Q:12 K:13
	const unsigned char _suit;  //0:black peach 1:red peach 2:plum 3:block 4:joke
	const unsigned char _score; 

	BASE_POKE_CARD(unsigned char __guid);
};
typedef BASE_POKE_CARD* LP_BASE_POKE_CARD;
typedef std::list<LP_BASE_POKE_CARD> POKE_LIST;

#define MAX_CARD_COUNT 52
class CPokeCard
{
protected:
	LP_BASE_POKE_CARD m_card[MAX_CARD_COUNT];

	POKE_LIST         m_cardPool;              //δ�������
	LP_BASE_POKE_CARD m_currentCard[MAX_CARD_COUNT];

	void reShuffle_cards();

public:
	CPokeCard(void);
	~CPokeCard(void);

	void Shuffle_cards();                      //ϴ��	

	const LP_BASE_POKE_CARD* get_card_array();
	void get_card_list(POKE_LIST& cardlist);
};

typedef MemAllocPool<CPokeCard> ALLOC_POKECARD;

#endif