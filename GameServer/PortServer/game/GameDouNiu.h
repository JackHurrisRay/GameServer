#ifndef _GAMEDOUNIU_H
#define _GAMEDOUNIU_H

/************************************************************************/
/* ��ģ���߼�Ϊ������Ϸģ���߼������Է���                                  */
/************************************************************************/
#include "Player.h"
#include "PokeCard.h"

enum ENUM_GAME_STATUS
{
	EGS_NONE = 0,
	EGS_FIGHT_FOR_ZHUANG,                                                   //��ׯ ����ע����ߵ�          ����Ϊ��ׯ�׶� ��ʼ��ע
	EGS_DEALER,                                                             //���� 5��ȫ��������ѡ��ţ��
	EGS_NIU,                                                                //ѡ����ţ�ƣ������ٴ�ѡ�񷭱��� ���ʼ�ȽϽ��


};

struct GAME_DOU_NIU
{
	//////////////////////////////////////////////////////////////////////////
	PLAYER_LIST      _player_list;                                          //����б�
	CPokeCard*       _PokeCard;                                             //��Ҫ��һ����

	ENUM_GAME_STATUS _game_status;

	
};

#endif