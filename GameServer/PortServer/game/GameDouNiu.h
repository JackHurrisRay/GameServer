#ifndef _GAMEDOUNIU_H
#define _GAMEDOUNIU_H

/************************************************************************/
/* 该模块逻辑为具体游戏模块逻辑，可以分离                                  */
/************************************************************************/
#include "Player.h"
#include "PokeCard.h"

//房间的状态
enum ENUM_GAME_STATUS
{
	EGS_NONE = 0,
	EGS_FIGHT_FOR_ZHUANG,                                                   //抢庄 以下注最大者得          或者为轮庄阶段 开始下注
	EGS_DEALER,                                                             //发牌 5张全发，并且选择牛牌
	EGS_NIU,                                                                //选三张牛牌，并且再次选择翻倍， 随后开始比较结果


};

//房间内玩家的状态
enum ENUM_PLAYER_STATUS
{
	EPS_NONE = 0,
	EPS_READY,                                                              //玩家准备状态，可以点开始了
	EPS_FIGHT_FOR_ZHUANG,                                                   //玩家抢庄状态，此时玩家开始下注抢庄
	EPS_DEALER,                                                             //抢完庄以后， 玩家进入发牌状态，此时，玩家可以选牛并且下注
	EPS_RESULT,                                                             //玩家开始算牛，结果发放客户端

};

//
enum ENUM_WIN_CARD_TYPE
{
	EWCT_NONE = 0,

	EWCT_NIU1,
	EWCT_NIU2,
	EWCT_NIU3,
	EWCT_NIU4,
	EWCT_NIU5,
	EWCT_NIU6,
	EWCT_NIU7,
	EWCT_NIU8,
	EWCT_NIU9,

	EWCT_NIUNIU,
	
	EWCT_SIHUANIU,
	EWCT_WUHUANIU,
	EWCT_WUXIAONIU,
	EWCT_ZHADAN,

};

//////////////////////////////////////////////////////////////////////////
struct PLAYER_DATA
{
	BASE_PLAYER*        _player;
	ENUM_PLAYER_STATUS  _status;
	unsigned char       _zhuang;                                            //庄
	unsigned char       _double;                                            //翻倍

	BASE_POKE_CARD*     _card[5];                                           //手牌

	//////////////////////////////////////////////////////////////////////////
	ENUM_WIN_CARD_TYPE process_WinCard(BASE_POKE_CARD*& _flagCard);

};
typedef MemAllocPool<PLAYER_DATA> ALLOC_PLAYER_DATA;
typedef std::list<PLAYER_DATA*> PLAYER_DATA_LIST;

//////////////////////////////////////////////////////////////////////////
struct GAME_DOU_NIU
{
	//////////////////////////////////////////////////////////////////////////
	static ALLOC_POKECARD _ALLOC_POKECARD;

	//////////////////////////////////////////////////////////////////////////
	PLAYER_DATA_LIST _player_list;                                          //玩家列表

	CPokeCard*       _PokeCard;                                             //需要有一副牌

	ENUM_GAME_STATUS _game_status;
	unsigned char    _zhuang_double;                                        //庄倍数

	//////////////////////////////////////////////////////////////////////////
	GAME_DOU_NIU();
	~GAME_DOU_NIU();

	//////////////////////////////////////////////////////////////////////////
	void player_ready(BASE_PLAYER* _player);                                //初始化玩家数据
	void fight_zhuang(BASE_PLAYER* _player);                                //抢庄
	void make_double(BASE_PLAYER* _player);                                 //加倍


	//////////////////////////////////////////////////////////////////////////
	void take_poke_to_players();                                            //发牌



};

#endif