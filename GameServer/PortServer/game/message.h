#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "./../core/ProtocalFactory.h"
#include "protocal.h"

#define JSON_FLAG                        "flag"
#define JSON_PLAYER_KEY                  "player_key"
#define JSON_PLAYER_IMG                  "player_img"
#define JSON_PLAYER_UID                  "player_id"
#define JSON_PLAYER_SCORE                "player_score"

#define JSON_PASSWORD                    "password"

#define JSON_ROOM_ID                     "room_id"
#define JSON_ROOM_RANDKEY                "room_rand_key"
#define JSON_ROOM_TYPE                   "room_type"
#define JSON_ROOM_STATUS                 "room_status"
#define JSON_ROOM_PLAYERCOUNT            "room_player_count"

#define JSON_STATUS                      "STATUS"

struct MSG_C2S_LOGIN:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_LOGIN():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_LOGIN)
	{
		//key值利用微信登录ID
		initializeParam(2, JSON_PLAYER_KEY, JSON_PLAYER_IMG, "", "");
	}
};

struct MSG_S2C_LOGIN:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_LOGIN():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_LOGIN)
	{
		initializeParam(3, JSON_PLAYER_UID, JSON_PLAYER_SCORE, JSON_ROOM_ID);
	}
};
 
struct MSG_C2S_CREATE_ROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_CREATE_ROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_CREATE_ROOM)
	{
		initializeParam(2, JSON_ROOM_TYPE, JSON_PASSWORD);
	}
};

struct MSG_S2C_CREATE_ROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_CREATE_ROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_CREATE_ROOM)
	{
		initializeParam(4, JSON_ROOM_ID, JSON_ROOM_RANDKEY, JSON_ROOM_TYPE, JSON_ROOM_STATUS);
	}
};

struct MSG_C2S_ENTER_ROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_ENTER_ROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_ENTER_ROOM)
	{
		initializeParam(2, JSON_ROOM_ID, JSON_PASSWORD);
	}
};

struct MSG_S2C_ENTER_ROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_ENTER_ROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_ALL_ENTER_ROOM)
	{
		initializeParam(2, JSON_ROOM_ID, JSON_PLAYER_UID);
	}
};

struct MSG_C2S_LEAVE_ROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_LEAVE_ROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_LEAVE_ROOM)
	{

	}
};

struct MSG_S2C_LEAVE_ROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_LEAVE_ROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_ALL_LEAVE_ROOM)
	{
		initializeParam(2, JSON_PLAYER_UID, JSON_ROOM_ID);
	}
};

struct MSG_C2S_DISBAND_ROOM_BY_OWNER:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_DISBAND_ROOM_BY_OWNER():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_DISBAND_ROOM_BY_OWNER)
	{
		initializeParam(1, JSON_ROOM_ID);
	}
};

struct MSG_S2C_DISBAND_ROOM_BY_OWNER:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_DISBAND_ROOM_BY_OWNER():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_DISBAND_ROOM_BY_OWNER)
	{
		initializeParam(1, JSON_ROOM_ID);
	}
};

struct MSG_C2S_REQUEST_ROOMLIST:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_REQUEST_ROOMLIST():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_REQUEST_ROOMLIST)
	{
		
	}
};

struct MSG_S2C_REQUEST_ROOMLIST:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_REQUEST_ROOMLIST():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_REQUEST_ROOMLIST)
	{
		initializeParam(3, JSON_ROOM_ID, JSON_ROOM_RANDKEY, JSON_ROOM_PLAYERCOUNT);
	}

};

#endif