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
	EGP_C2S_DISBAND_ROOM           = EGP_START__LOGIN + 26,         //解散房间请求
	EPG_S2C_ALL_DISBAND_ROOM       = EGP_START__LOGIN + 27,         //

	EGP_C2S_DISBAND_RESPONSE       = EGP_START__LOGIN + 28,         //解散房间响应
	EGP_S2C_ALL_DISBAND_RESPONSE   = EGP_START__LOGIN + 29,

	////
	EGP_C2S_DISBAND_ROOM_BY_OWNER  = EGP_START__LOGIN + 30,
	EGP_S2C_DISBAND_ROOM_BY_OWNER  = EGP_START__LOGIN + 31,

	////
	EGP_C2S_REQUEST_ROOMLIST       = EGP_START__LOGIN + 40,         //房主获取所开房间信息
	EGP_S2C_REQUEST_ROOMLIST       = EGP_START__LOGIN + 41,

	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	//////////////////////////////////////////////////////////////////////////
	EGP_C2S_STARTGAME              = EGP_START__GAME + 10,          //开始游戏
	EGP_S2C_ALL_STARTGAME          = EGP_START__GAME + 11, 



};

#endif