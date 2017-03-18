#ifndef _PLAYER_H
#define _PLAYER_H
#include "./../core/MemAllocPool.h"
#include "game_common.h"

struct BASE_OBJECT;
struct BASE_PLAYER
{
	//////////////////////////////////////////////////////////////////////////
	char                _KEY[MAX_KEY_COUNT];                                //���Ψһ��ʶ

	unsigned short      _PLAYER_ID;                                         //���ID����ϵͳ����, ��ʶΪ��ҵ�UID
	short               _ROOMID;                                            //��ҷ����ʶ 0Ϊ������

	unsigned long long  _GOLD;                                              //���
	long long           _SCORE;                                             //����
	BASE_OBJECT*        _CLIENT;                                            //

	char                _INDEX;                                             //-1Ϊ�����ã� ���������λ˳��

	void initData();

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

