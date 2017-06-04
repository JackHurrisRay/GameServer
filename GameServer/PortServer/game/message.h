#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "./../core/ProtocalFactory.h"
#include "protocal.h"


#define GAME_VERSION                     "1.0.0"
#define JSON_VERSION                     "version"

#define JSON_TIME                        "time"
#define JSON_FLAG                        "flag"
#define JSON_ZHUANG                      "zhuang"
#define JSON_ZHUANG_VALUE                "zhuang_value"
#define JSON_DOUBLE                      "double"
#define JSON_POKECARD                    "pokecard"

#define JSON_BASEDOUBLE                  "basedouble"
#define JSON_ZHUANGDOUBLE                "zhuangdouble"

//////////////////////////////////////////////////////////////////////////
#define JSON_PLAYER                      "player"

#define JSON_PLAYER_KEY                  "player_key"
#define JSON_PLAYER_NICKNAME             "player_nickname"
#define JSON_PLAYER_IMG                  "player_img"
#define JSON_PLAYER_UID                  "player_id"
#define JSON_PLAYER_DIAMOND              "player_diamond"
#define JSON_PLAYER_GOLD                 JSON_PLAYER_DIAMOND

#define JSON_PLAYER_ZHUANG               "player_zhuang"
#define JSON_PLAYER_CURRENTSCORE         "current_score"
#define JSON_PLAYER_TOTALSCORE           "total_score"
#define JSON_POKECARD_WINTYPE            "pokecard_wintype"

#define JSON_PLAYER_INDEXINROOM          "player_index_in_room"
#define JSON_PLAYER_GAMESTATUS           "player_game_status"

#define JSON_PLAYER_LEAVE_STATUS         "leave_status"

#define JSON_PLAYER_VIP                  "vip"
#define JSON_PLAYER_VIP_STARTTIME        "vip_start"
#define JSON_PLAYER_IS_VIP               "is_vip"
#define JSON_MAX_ROOM_CANBECREATED       "max_room_created"

#define JSON_IMG_DATA                    "img_data"
#define JSON_PASSWORD                    "password"
#define JSON_BASESCORE                   "basescore"
#define JSON_TOTALSCORE                  "totalscore"

#define JSON_ROOM_ID                     "room_id"
#define JSON_ROOM_RANDKEY                "room_rand_key"
#define JSON_ROOM_TYPE                   "room_type"
#define JSON_ROOM_STATUS                 "room_status"
#define JSON_ROOM_PLAYERCOUNT            "room_player_count"
#define JSON_ROOM_CURRENT_AROUND         "room_current_around"
#define JSON_ROOM_MAX_AROUND             "room_max_around"
#define JSON_ROOM_PLAYERSINFO_DATA       "room_players_info"
#define JSON_ROOM_GAMESTATUS             "room_game_status"
#define JSON_ROOM_APPLCATELEAVE          "room_applicate_leave"

#define JSON_ROOM_START_TIME             "room_start_time"
#define JSON_ROOM_END_TIME               "room_end_time"

#define JSON_LAST_GAMERESULT             "last_gameresult"

#define JSON_EX_DATA                     "extern_data"
#define JSON_STATUS                      "STATUS"

//////////////////////////////////////////////////////////////////////////
struct MSG_C2S_HEART:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_HEART():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_HEART)
	{
		initializeParam(2, JSON_FLAG, JSON_TIME);
	}
};

struct MSG_S2C_HEART:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_HEART():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_HEART)
	{
		initializeParam(2, JSON_FLAG, JSON_TIME);
	}
};

//////////////////////////////////////////////////////////////////////////
struct MSG_C2S_LOGIN:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_LOGIN():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_LOGIN)
	{
		//key值利用微信登录ID
		initializeParam(2, JSON_PLAYER_KEY, JSON_PLAYER_NICKNAME);
	}
};

struct MSG_S2C_LOGIN:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_LOGIN():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_LOGIN)
	{
		initializeParam(16, JSON_PLAYER_UID, JSON_PLAYER_DIAMOND, 
			JSON_PLAYER_KEY, JSON_PLAYER_NICKNAME, 
			JSON_ROOM_ID, JSON_ROOM_RANDKEY, JSON_PLAYER_INDEXINROOM, JSON_BASESCORE,
			JSON_ROOM_GAMESTATUS, JSON_PLAYER_GAMESTATUS,
			JSON_EX_DATA, JSON_ROOM_PLAYERSINFO_DATA,
			JSON_PLAYER_VIP, JSON_PLAYER_VIP_STARTTIME, 
			JSON_PLAYER_IS_VIP,JSON_VERSION
			);
	}
};

struct MSG_C2S_GETPLAYERDATA:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_GETPLAYERDATA():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_GETPLAYERDATA)
	{

	}
};

struct MSG_S2C_GETPLAYERDATA:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_GETPLAYERDATA():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_GETPLAYERDATA)
	{
		initializeParam(1, JSON_PLAYER_GOLD);
	}
};

//////////////////////////////////////////////////////////////////////////
struct MSG_C2S_CREATE_ROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_CREATE_ROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_CREATE_ROOM)
	{
		initializeParam(4, JSON_ROOM_TYPE, JSON_PASSWORD, JSON_ROOM_MAX_AROUND, JSON_BASESCORE);
	}
};

struct MSG_S2C_CREATE_ROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_CREATE_ROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_CREATE_ROOM)
	{
		initializeParam(6, 
			JSON_ROOM_ID, JSON_ROOM_RANDKEY, JSON_ROOM_TYPE, JSON_ROOM_STATUS, JSON_ROOM_MAX_AROUND, 
			JSON_BASESCORE);
	}
};

struct MSG_C2S_ENTER_ROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_ENTER_ROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_ENTER_ROOM)
	{
		initializeParam(2, JSON_ROOM_RANDKEY, JSON_PASSWORD);
	}
};

struct MSG_S2C_ENTER_ROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_ENTER_ROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_ALL_ENTER_ROOM)
	{
		initializeParam(10, 
			JSON_ROOM_ID, JSON_ROOM_RANDKEY, JSON_PLAYER_UID, JSON_PLAYER_KEY, JSON_PLAYER_NICKNAME, JSON_PLAYER_INDEXINROOM,
			JSON_BASESCORE, JSON_ROOM_GAMESTATUS, JSON_PLAYER_GAMESTATUS,
			JSON_ROOM_PLAYERSINFO_DATA);
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
		initializeParam(3, JSON_PLAYER_UID, JSON_ROOM_ID, JSON_PLAYER_GOLD);
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
		initializeParam(1, JSON_EX_DATA);
	}

};

//////////////////////////////////////////////////////////////////////////
struct MSG_C2S_REQUEST_PLAYERSINROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_REQUEST_PLAYERSINROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_REQUEST_PLAYERSINROOM)
	{

	}
};

struct MSG_S2C_REQUEST_PLAYERINROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_REQUEST_PLAYERINROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_REQUEST_PLAYERSINROOM)
	{
		initializeParam(2, JSON_ROOM_ID, JSON_EX_DATA);
	}
};


//////////////////////////////////////////////////////////////////////////
struct MSG_C2S_FINDINVITER_INROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_FINDINVITER_INROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_FINDINVITER_INROOM)
	{
		initializeParam(1, JSON_PLAYER_KEY);
	}
};

struct MSG_S2C_FINDINVITER_INROOM:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_FINDINVITER_INROOM():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_FINDINVITER_INROOM)
	{
		initializeParam(1, JSON_ROOM_RANDKEY);
	}
};

//////////////////////////////////////////////////////////////////////////
struct MSG_C2S_PAY_VIP:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_PAY_VIP():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_PAY_VIP)
	{
		initializeParam(1, JSON_PLAYER_VIP);
	}
};

struct MSG_S2C_PAY_VIP:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_PAY_VIP():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_PAY_VIP)
	{
		initializeParam(3, JSON_PLAYER_VIP, JSON_PLAYER_VIP_STARTTIME, JSON_PLAYER_IS_VIP);
	}
};

//////////////////////////////////////////////////////////////////////////
struct MSG_C2S_GET_LASTRESULT:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_GET_LASTRESULT():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_GET_LASTRESULT)
	{

	}
};

struct MSG_S2C_GET_LASTRESULT:
	public BASE_PROTOCAL_MSG
{
	MSG_S2C_GET_LASTRESULT():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_S2C_GET_LASTRESULT)
	{
		initializeParam(1, JSON_EX_DATA);
	}
};


#endif