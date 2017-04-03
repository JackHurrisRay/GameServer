#ifndef _PLAYER_H
#define _PLAYER_H
#include "./../core/MemAllocPool.h"
#include "game_common.h"

#include "GameDouNiu.h"

enum ENUM_PLAYER_TYPE
{
	EPT_NONE = 0,
	EPT_DAY,                                                                //�����û�
	EPT_WEEK,                                                               //�����û�
	EPT_MONTH,                                                              //�����û�
};

struct BASE_OBJECT;
struct BASE_PLAYER:
	public GAME_PLAYER_DATA
{
	//////////////////////////////////////////////////////////////////////////
	char                _KEY[MAX_KEY_COUNT];                                //���Ψһ��ʶ
	char                _NickName[MAX_NICKNAME_COUNT];                      //����ǳ�

	unsigned short      _PLAYER_ID;                                         //���ID����ϵͳ����, ��ʶΪ��ҵ�UID
	short               _ROOMID;                                            //��ҷ����ʶ 0Ϊ������

	unsigned long long  _GOLD;                                              //���
	ENUM_PLAYER_TYPE    _EPT_TYPE;                                          //


	//////////////////////////////////////////////////////////////////////////
	BASE_OBJECT*        _CLIENT;                                            //

	char                _INDEX;                                             //-1Ϊ�����ã� ���������λ˳��
	std::string         _PLAYER_DATA_PATH;

	//////////////////////////////////////////////////////////////////////////
	void initData();

	void release();


	//////////////////////////////////////////////////////////////////////////
	void saveData();
	bool loadData();

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

};


#endif

