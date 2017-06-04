#ifndef _PLAYER_H
#define _PLAYER_H
#include "./../core/MemAllocPool.h"
#include "game_common.h"

#include "GameDouNiu.h"

enum ENUM_PLAYER_TYPE
{
	EPT_NONE = 0,
	EPT_DAY,                                                                //包日用户
	EPT_WEEK,                                                               //包周用户
	EPT_MONTH,                                                              //包月用户
};

enum ENUM_ERROR_PLAYER_TYPE
{
	EEPT_OK = 0,
	EEPT_ALREADYVIP,
	EEPT_FEWGOLD,
};

struct CHEAT_DATA
{

};

#define MAX_ROOM_CANBE_CREATED 3

struct BASE_OBJECT;
struct BASE_PLAYER:
	public GAME_PLAYER_DATA
{
	//////////////////////////////////////////////////////////////////////////
	char                _KEY[MAX_KEY_COUNT];                                //玩家唯一标识
	char                _NickName[MAX_NICKNAME_COUNT];                      //玩家昵称

	unsigned short      _PLAYER_ID;                                         //玩家ID，由系统生成, 标识为玩家的UID
	short               _ROOMID;                                            //玩家房间标识 0为不可用

	unsigned long long  _GOLD;                                              //金币

	ENUM_PLAYER_TYPE    _EPT_TYPE;                                          //
	time_t              _VIP_START_TIME;                                    

	int                 _MAX_ROOM_COUNT;                                    //房间最大数，默认为3


	//////////////////////////////////////////////////////////////////////////
	BASE_OBJECT*        _CLIENT;                                            //

	char                _INDEX;                                             //-1为不可用， 房间里的座位顺序
	std::string         _PLAYER_DATA_PATH;
	std::string         _PLAYER_LAST_RESULT;                                //最后一次游戏结果

	//////////////////////////////////////////////////////////////////////////
	

	//////////////////////////////////////////////////////////////////////////
	void initData();

	void release();


	//////////////////////////////////////////////////////////////////////////
	void saveData();
	bool loadData();

	bool isVIP();

};
typedef BASE_PLAYER* LP_BASE_PLAYER;
typedef MemAllocPool<BASE_PLAYER> ALLOC_PLAYER;
typedef std::list<BASE_PLAYER*> PLAYER_LIST;

class Players
{
protected:
	Players(void);
	~Players(void);
	
	static ALLOC_PLAYER _ALLOC_PLAYER;
	LP_BASE_PLAYER*     m_players;
	Locker              m_lock;

	friend class PLAYER_LOCK;

public:
	static Players* Instance()
	{
		static Players _players;
		return &_players;
	}

	//////////////////////////////////////////////////////////////////////////
	BASE_PLAYER* login_success(unsigned short& _uid, const char* _key);
	bool check_player(unsigned short _uid);

	//////////////////////////////////////////////////////////////////////////
	bool refresh_player(unsigned short& _uid, const char* _key);

	//////////////////////////////////////////////////////////////////////////
	void delete_player(unsigned short _uid);
	BASE_PLAYER* get_player(unsigned short _uid);
	int getPlayerCountOnLine();
};


#endif

