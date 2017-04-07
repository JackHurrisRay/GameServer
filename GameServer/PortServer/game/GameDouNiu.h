#ifndef _GAMEDOUNIU_H
#define _GAMEDOUNIU_H

/************************************************************************/
/* ��ģ���߼�Ϊ������Ϸģ���߼������Է���                                  */
/************************************************************************/
#include "./../core/ProtocalFactory.h"
#include "PokeCard.h"

struct BASE_PLAYER;

/************************************************************************
ENTER ROOM : Player -> EPS_NONE
READY GAME : Player -> EPS_READY  if cancel Player -> EPS_NONE
WHEN ALL Players -> EPS_READY GAME START



************************************************************************/
//��������ҵ�״̬
enum ENUM_PLAYER_STATUS
{
	EPS_NONE = 0,
	EPS_READY,                                                              //���׼�����ˣ��ȴ�ȫ�嵽�����״̬�� ��Ϸ�Զ���ʼȻ����Կ�ʼ��ׯ
	EPS_FIGHT_FOR_ZHUANG,                                                   //�������ׯ���ȴ�ȫ�嵽�����״̬, Ȼ����Կ�ʼ����
	EPS_DEALER,                                                             //�������ˣ���ҿ��Լӱ�
	//EPS_RESULT,                                                             //�ӱ����ˣ�����ҵȴ���Ϸ�������Ϸ��������Ժ�״̬����ΪEPS_NONE

	EPS_MAX,                                                                //��ʶ����û���κ�����
};

//��������
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

//��Ϸ�����д���
enum ENUM_GAME_STATUS_ERROR
{
	EGSE_OK = 0,
	EGSE_UNKNOWN = 100,                   //δ֪����
	EGSE_DATA_ERROR,                      //���ݴ���
	EGSE_CURRENT_STATUS_ERROR,            //��ǰ״̬����ȷ
	EGSE_RIGHT_LIMITED,                   //��ǰȨ�޲���ȷ
};

#define MAX_CARD_PER_PLAYER 5
//////////////////////////////////////////////////////////////////////////
struct GAME_PLAYER_DATA
{
	ENUM_PLAYER_STATUS  _status;

	long long           _currentScore;                                      //��ǰ�ַ���
	long long           _totalSCORE;                                        //��ҷ���

	unsigned char       _zhuang;                                            //ׯ��ֵ
	unsigned char       _double;                                            //����

	bool                _isZhuang;

	ENUM_WIN_CARD_TYPE  _winType;
	BASE_POKE_CARD*     _winCard;
	BASE_POKE_CARD*     _card[MAX_CARD_PER_PLAYER];                         //����

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
	CPokeCard*       _PokeCard;                                             //��Ҫ��һ����
	BASE_PLAYER*     _zhuangPlayer;
	unsigned char    _zhuangValue;                                          //ׯ
	unsigned char    _currentRound;                                         //��ǰ����

	//////////////////////////////////////////////////////////////////////////
	std::string      _INFO_playersPokeCard;

	//////////////////////////////////////////////////////////////////////////
	GAME_DOU_NIU();

	static bool getPlayerPokeCardInfo(BASE_ROOM* _room, std::string& _info, int _limit);
	static void computerPlayerScore(BASE_ROOM* _room, std::string& _info);
	static int  getWinTypeScore(BASE_PLAYER* _player);
};

#endif