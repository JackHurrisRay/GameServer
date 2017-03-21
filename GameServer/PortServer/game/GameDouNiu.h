#ifndef _GAMEDOUNIU_H
#define _GAMEDOUNIU_H

/************************************************************************/
/* ��ģ���߼�Ϊ������Ϸģ���߼������Է���                                  */
/************************************************************************/
#include "Player.h"
#include "PokeCard.h"

//�����״̬
enum ENUM_GAME_STATUS
{
	EGS_NONE = 0,
	EGS_FIGHT_FOR_ZHUANG,                                                   //��ׯ ����ע����ߵ�          ����Ϊ��ׯ�׶� ��ʼ��ע
	EGS_DEALER,                                                             //���� 5��ȫ��������ѡ��ţ��
	EGS_NIU,                                                                //ѡ����ţ�ƣ������ٴ�ѡ�񷭱��� ���ʼ�ȽϽ��


};

//��������ҵ�״̬
enum ENUM_PLAYER_STATUS
{
	EPS_NONE = 0,
	EPS_READY,                                                              //���׼��״̬�����Ե㿪ʼ��
	EPS_FIGHT_FOR_ZHUANG,                                                   //�����ׯ״̬����ʱ��ҿ�ʼ��ע��ׯ
	EPS_DEALER,                                                             //����ׯ�Ժ� ��ҽ��뷢��״̬����ʱ����ҿ���ѡţ������ע
	EPS_RESULT,                                                             //��ҿ�ʼ��ţ��������ſͻ���

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
	unsigned char       _zhuang;                                            //ׯ
	unsigned char       _double;                                            //����

	BASE_POKE_CARD*     _card[5];                                           //����

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
	PLAYER_DATA_LIST _player_list;                                          //����б�

	CPokeCard*       _PokeCard;                                             //��Ҫ��һ����

	ENUM_GAME_STATUS _game_status;
	unsigned char    _zhuang_double;                                        //ׯ����

	//////////////////////////////////////////////////////////////////////////
	GAME_DOU_NIU();
	~GAME_DOU_NIU();

	//////////////////////////////////////////////////////////////////////////
	void player_ready(BASE_PLAYER* _player);                                //��ʼ���������
	void fight_zhuang(BASE_PLAYER* _player);                                //��ׯ
	void make_double(BASE_PLAYER* _player);                                 //�ӱ�


	//////////////////////////////////////////////////////////////////////////
	void take_poke_to_players();                                            //����



};

#endif