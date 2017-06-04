#ifndef _GAMEROOM_H
#define _GAMEROOM_H

/************************************************************************/
/* 游戏房间                                                              */
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
#include "./../core/MemAllocPool.h"
#include <list>
#include "Player.h"
#include "game_common.h"

//////////////////////////////////////////////////////////////////////////
#include "GameDouNiu.h"

//////////////////////////////////////////////////////////////////////////
enum ENUM_ROOM_ERROR
{
	ERE_ROOM_OK                  = 0,
	ERE_ROOM_UNKNOWN             = 100,
	ERE_ROOM_CAN_NOT_CREATED     = 101,       //无法创建房间
	ERE_ROOM_CAN_NOT_CREATED_WITHOUTPAY,      //由于金币不足无法创建房间
	ERE_ROOM_IS_NOT_EXIST,                    //房间不存在
	ERE_ROOM_PLAYER_OUT_ROOM,                 //玩家不在任何房间内
	ERE_ROOM_PLAYER_IN_HERE,                  //玩家本来就存在于该房间内
	ERE_ROOM_PLAYER_IN_OTHER_ROOM,            //玩家在其他房间内
	ERE_ROOM_IN_GAME,                         //房间当前状态为游戏中状态
	ERE_ROOM_IS_FULL,                         //房间已满
	ERE_ROOM_SOMEPLAYERINHERE,                //房间中有人
	ERE_ROOM_PASSWORD_TYPE,                   //密码格式
	ERE_ROOM_WRONG_PASSWORD,                  //密码错误
	ERE_ROOM_LIMITRIGHT,                      //房间权限问题
	ERE_ROOM_WRONG_STATUS,                    //房间错误的状态
	ERE_ROOM_MAX_COUNT_LIMITED,               //开房数目收到限制
	ERE_ROOM_UNKNOWN_THROW       = 9999,      //无效消息返回，直接丢弃
};

//////////////////////////////////////////////////////////////////////////
enum ENUM_GAME_ROOM_TYPE
{
	EGRT_NONE = 0,          //该类型不可用

	EGRT_TURN_BANKER,       //轮流庄家
	EGRT_FIGHT_BANKER,      //抢庄家
	EGRT_NONE_BANKER,       //无庄家

	EGRT_COUNT,
};

enum ENUM_ROOM_PLAYER_ONLINE
{
	ERPO_NONE = 0,          //正常状态
	ERPO_ALLLEAVE,          //玩家都离开了
};

//////////////////////////////////////////////////////////////////////////
enum ENUM_ROOM_STATUS
{
	ERS_NONE = 0,
	ERS_NOGAME,            //房间被创建的状态，但是没有开始游戏
	ERS_INGAME,            //该状态下，房间不可解散加入或者离开，只能申请大家一致解散房间
};

struct BASE_ROOM:
	public GAME_DOU_NIU
{
	//////////////////////////////////////////////////////////////////////////
	char           _OWNER_KEY[MAX_KEY_COUNT];            //房间创建者的KEY ID
	char           _PASSWORD[MAX_PASSWORD_COUNT];        //若都为0，则无密码，若有密码，则需要密码验证才能生成

	//////////////////////////////////////////////////////////////////////////
	const int      _ROOM_ID_RANDFLAG;                    //6位数值的随机种子

	unsigned short _ROOM_ID;                             //房间uid 范围0~MAX_ROOM_LIMIT
	short          _OWNER_UID;                           //房间创建者uid


	unsigned char  _MAX_ROUND;                           //最大局数                     
	unsigned char  _BANKER_TYPE;                         //玩法类型 


	//////////////////////////////////////////////////////////////////////////
	unsigned char  _baseScore;                           //基础分


	time_t         _start_time;                          //房间创建时间
	time_t         _agree_disband;                       //房间解散等待同意的时间
	time_t         _check_online_time;

	//////////////////////////////////////////////////////////////////////////
	ENUM_GAME_ROOM_TYPE       _room_type;       
	ENUM_ROOM_STATUS          _room_status;
	ENUM_ROOM_PLAYER_ONLINE   _room_player_online;

	//////////////////////////////////////////////////////////////////////////	
	BASE_PLAYER*          _Players[MAX_PLAYER_IN_ROOM];                     //
	unsigned char         _AgreeDisband[MAX_PLAYER_IN_ROOM];                //0:初始状态 1:同意 2不同意
	int                   _AgreeDisbandStatus;                              //0:普通状态 1:正在申请退出房间

	BASE_ROOM();
	void initData();

	//////////////////////////////////////////////////////////////////////////
	bool isNeedPassword();

	//////////////////////////////////////////////////////////////////////////
	//房间人员全部退出
	ENUM_ROOM_ERROR playersAllLeave();

	//房间被解散
	ENUM_ROOM_ERROR disband();

	//////////////////////////////////////////////////////////////////////////
	//0:都不做声 1:至少有一个赞同并且没有不赞同 2:至少有一个不赞同
	int  checkPlayerAgreeToLeave();
	bool checkPlayersOnLine();

	//
	void clearAgreeToLeave();
	int  getCurrentPlayersCount();
	void getPlayersInRoom(PLAYER_LIST& _playerList);
	int  getExDataFromApplicateLeave(std::string& _exData);
	int  getExDataFromApplicateLeave(Json::Value& _root);

	void getStringOfRoomResultData(std::string& _strOut);

	//////////////////////////////////////////////////////////////////////////
	template<class T>
	void brodcast(T msg, BASE_PLAYER* _exceptplayer = NULL)
	{
		for( int i=0; i<MAX_PLAYER_IN_ROOM; i++ )
		{
			BASE_PLAYER* _player = _Players[i];
			if( _player != NULL && _player->_CLIENT && _player != _exceptplayer )
			{
				//SEND_MSG(msg, _player->_CLIENT, OP_BRODCAST);
				PUSH_MSG(msg, _player->_CLIENT);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	bool getPlayersInfo(std::string& _info);
};
typedef BASE_ROOM* LP_BASE_ROOM;
typedef std::list<LP_BASE_ROOM> ROOM_LIST;
typedef MemAllocPool<BASE_ROOM> ALLOC_ROOM;

class GameRooms
{
protected:
	GameRooms(void);
	~GameRooms(void);

	static ALLOC_ROOM _ALLOC_ROOM;
	LP_BASE_ROOM*     m_rooms;
	Locker            m_lock;
	time_t            m_currentTime;

	friend class ROOM_LOCK;

public:
	static GameRooms* Instance()
	{
		static GameRooms _rooms;
		return &_rooms;
	}

	BASE_ROOM* get_room(short room_id);

	//创建房间 0:成功创建房间 1:金币不足无法创建房间 2:条件不足无法创建房间 3:房间已满，无法创建房间       >0xFF:其他原因无法创建房间
	ENUM_ROOM_ERROR createRoom(BASE_PLAYER* _player, BASE_ROOM*& _room);
	//进入房间 0:成功进入房间 1:房间已满，无法进入房间 2:条件不足无法进入房间 3:已经在房间内，无法进入新房间 0xF:房间不存在无法进入 >0xFF:其他原因无法进入房间
	ENUM_ROOM_ERROR enterRoom(BASE_PLAYER* _player, const char* _password, BASE_ROOM* _room);
	ENUM_ROOM_ERROR enterRoom(int _rand_key, BASE_PLAYER* _player, const char* _password, BASE_ROOM*& _room);
	//离开房间 0:成功离开房间 1:游戏中无法离开房间 >0xF:其他原因无法离开房间 0xF:房间不存在无法离开 >0xFF:其他原因无法离开
	ENUM_ROOM_ERROR leaveRoom(BASE_PLAYER* _player, BASE_ROOM*& _room);
	//全部离开房间 0:成功解散房间 1:有人不同意解散房间 2:房间状态不允许解散房间 0xF:房间不存在无法解散 >0xFF:其他原因无法解散房间
	ENUM_ROOM_ERROR leaveGameRoom(bool _agree, BASE_PLAYER* _player, BASE_ROOM*& _room);
	//解散房间 0:成功解散房间 1:有人正在游戏中，不可解散房间
	ENUM_ROOM_ERROR disbandRoomByOwner(short _room_id, BASE_PLAYER* _player);

	////
	void disbandRoomAfterGameOver(short _room_id);

	//////////////////////////////////////////////////////////////////////////
	void updateRooms();

	void updateForApplicateLeave(BASE_ROOM* _room);
	void updateForTimeoutDisbandRoom(BASE_ROOM* _room);

	void getRoomsByOwner(const unsigned short _player_id, ROOM_LIST& _room_list);
};

#endif