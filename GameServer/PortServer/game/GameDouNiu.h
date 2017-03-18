#ifndef _GAMEDOUNIU_H
#define _GAMEDOUNIU_H

/************************************************************************/
/* 该模块逻辑为具体游戏模块逻辑，可以分离                                  */
/************************************************************************/
#include "Player.h"
#include "PokeCard.h"

enum ENUM_GAME_STATUS
{
	EGS_NONE = 0,
	EGS_FIGHT_FOR_ZHUANG,                                                   //抢庄 以下注最大者得          或者为轮庄阶段 开始下注
	EGS_DEALER,                                                             //发牌 5张全发，并且选择牛牌
	EGS_NIU,                                                                //选三张牛牌，并且再次选择翻倍， 随后开始比较结果


};

struct GAME_DOU_NIU
{
	//////////////////////////////////////////////////////////////////////////
	PLAYER_LIST      _player_list;                                          //玩家列表
	CPokeCard*       _PokeCard;                                             //需要有一副牌

	ENUM_GAME_STATUS _game_status;

	
};

#endif