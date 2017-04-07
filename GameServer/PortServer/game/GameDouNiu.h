#ifndef _GAMEDOUNIU_H
#define _GAMEDOUNIU_H

/************************************************************************/
/* 该模块逻辑为具体游戏模块逻辑，可以分离                                  */
/************************************************************************/
#include "./../core/ProtocalFactory.h"
#include "PokeCard.h"

struct BASE_PLAYER;

/************************************************************************
ENTER ROOM : Player -> EPS_NONE
READY GAME : Player -> EPS_READY  if cancel Player -> EPS_NONE
WHEN ALL Players -> EPS_READY GAME START



************************************************************************/
//房间内玩家的状态
enum ENUM_PLAYER_STATUS
{
	EPS_NONE = 0,
	EPS_READY,                                                              //玩家准备好了，等待全体到达这个状态， 游戏自动开始然后可以开始抢庄
	EPS_FIGHT_FOR_ZHUANG,                                                   //玩家抢完庄，等待全体到达这个状态, 然后可以开始发牌
	EPS_DEALER,                                                             //发完牌了，玩家可以加倍
	//EPS_RESULT,                                                             //加倍完了，则玩家等待游戏结果，游戏结果发还以后，状态重置为EPS_NONE

	EPS_MAX,                                                                //标识符，没有任何意义
};

//牌型种类
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
	EWCT_ZHANDANWUXIAO,

	EWCT_COUNT,
};

//游戏过程中错误
enum ENUM_GAME_STATUS_ERROR
{
	EGSE_OK = 0,
	EGSE_UNKNOWN = 100,                   //未知错误
	EGSE_DATA_ERROR,                      //数据错误
	EGSE_CURRENT_STATUS_ERROR,            //当前状态不正确
	EGSE_RIGHT_LIMITED,                   //当前权限不正确
};

#define MAX_CARD_PER_PLAYER 5
//////////////////////////////////////////////////////////////////////////
struct GAME_PLAYER_DATA
{
	ENUM_PLAYER_STATUS  _status;

	long long           _currentScore;                                      //当前局分数
	long long           _totalSCORE;                                        //玩家分数

	unsigned char       _zhuang;                                            //庄的值
	unsigned char       _double;                                            //翻倍

	bool                _isZhuang;

	ENUM_WIN_CARD_TYPE  _winType;
	BASE_POKE_CARD*     _winCard;
	BASE_POKE_CARD*     _card[MAX_CARD_PER_PLAYER];                         //手牌

	//////////////////////////////////////////////////////////////////////////
	ENUM_WIN_CARD_TYPE process_WinCard(BASE_POKE_CARD*& _flagCard);
	void resetData();

	//////////////////////////////////////////////////////////////////////////
	ENUM_GAME_STATUS_ERROR readyGame();
	ENUM_GAME_STATUS_ERROR cancelReady();
	ENUM_GAME_STATUS_ERROR fightForZhuang(int _value);
	ENUM_GAME_STATUS_ERROR doubleScore(int _double);

	void checkWinCard();

	//////////////////////////////////////////////////////////////////////////
	void getPokeCard(INTEGER_ARRAY& _pokecard_data);
};

//////////////////////////////////////////////////////////////////////////
struct BASE_ROOM;
struct GAME_DOU_NIU
{
	//////////////////////////////////////////////////////////////////////////
	static ALLOC_POKECARD _ALLOC_POKECARD;
	ENUM_PLAYER_STATUS    _PLAYERS_STATUS;

	//////////////////////////////////////////////////////////////////////////
	CPokeCard*       _PokeCard;                                             //需要有一副牌
	BASE_PLAYER*     _zhuangPlayer;
	unsigned char    _zhuangValue;                                          //庄
	unsigned char    _currentRound;                                         //当前局数

	//////////////////////////////////////////////////////////////////////////
	std::string      _INFO_playersPokeCard;

	//////////////////////////////////////////////////////////////////////////
	GAME_DOU_NIU();

	static bool getPlayerPokeCardInfo(BASE_ROOM* _room, std::string& _info, int _limit);
	static void computerPlayerScore(BASE_ROOM* _room, std::string& _info);
	static int  getWinTypeScore(BASE_PLAYER* _player);
};

#endif