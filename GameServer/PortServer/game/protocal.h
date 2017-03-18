#ifndef _PROTOCAL_H
#define _PROTOCAL_H

#define  EGP_START__LOGIN 2000
#define  EGP_START__GAME  3000

enum ENUM_GAME_PROTOCAL
{
	//////////////////////////////////////////////////////////////////////////
	EGP_NONE = 1001,

	//////////////////////////////////////////////////////////////////////////
	EGP_C2S_LOGIN           = EGP_START__LOGIN + 10,
	EGP_S2C_LOGIN           = EGP_START__LOGIN + 11,

	EGP_S2C_RECONNECT_HALL  = EGP_START__LOGIN + 12,
	EGP_S2C_RECONNECT_READY = EGP_START__LOGIN + 13,
	EGP_S2C_RECONNECT_ROOM  = EGP_START__LOGIN + 14,


	//////////////////////////////////////////////////////////////////////////
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//////////////////////////////////////////////////////////////////////////
	EGP_C2S_CREATE_ROOM    = EGP_START__LOGIN + 20,
	EGP_S2C_CREATE_ROOM    = EGP_START__LOGIN + 21,

	EGP_C2S_ENTER_ROOM     = EGP_START__LOGIN + 22,
	EGP_S2C_ALL_ENTER_ROOM = EGP_START__LOGIN + 23,

	EGP_C2S_LEAVE_ROOM     = EGP_START__LOGIN + 24,
	EGP_S2C_ALL_LEAVE_ROOM = EGP_START__LOGIN + 25,

	////
	EGP_C2S_DISBAND_ROOM           = EGP_START__LOGIN + 26,         //��ɢ��������
	EPG_S2C_ALL_DISBAND_ROOM       = EGP_START__LOGIN + 27,         //

	EGP_C2S_DISBAND_RESPONSE       = EGP_START__LOGIN + 28,         //��ɢ������Ӧ
	EGP_S2C_ALL_DISBAND_RESPONSE   = EGP_START__LOGIN + 29,

	////
	EGP_C2S_DISBAND_ROOM_BY_OWNER  = EGP_START__LOGIN + 30,
	EGP_S2C_DISBAND_ROOM_BY_OWNER  = EGP_START__LOGIN + 31,

	////
	EGP_C2S_REQUEST_ROOMLIST       = EGP_START__LOGIN + 40,         //������ȡ����������Ϣ
	EGP_S2C_REQUEST_ROOMLIST       = EGP_START__LOGIN + 41,

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//////////////////////////////////////////////////////////////////////////
	EGP_C2S_STARTGAME              = EGP_START__GAME + 10,          //��ʼ��Ϸ,����Ҽ��뵱ǰ�������Ϸ�б���
	EGP_S2C_ALL_STARTGAME          = EGP_START__GAME + 11, 

	EGP_C2S_FIGHT_ZHUANG           = EGP_START__GAME + 20,          //��ׯЭ�飬��ׯ�󲻿�����
	EGP_S2C_FIGHT_ZHUANG           = EGP_START__GAME + 21,

	EGP_C2S_ADD_DOUBLE_SCORE       = EGP_START__GAME + 22,          //�ӱ�

	EGP_S2C_GAME_SEND_POKECARD_3   = EGP_START__GAME + 31,          //����
	EGP_S2C_GAME_SEND_POKECARD_4   = EGP_START__GAME + 32,
	EGP_S2C_GAME_SEND_POKECARD_5   = EGP_START__GAME + 32,

	EGP_S2C_GAME_RESULT            = EGP_START__GAME + 41,          //���

};

#endif