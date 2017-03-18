#ifndef _GAMEROOM_H
#define _GAMEROOM_H

/************************************************************************/
/* ��Ϸ����                                                              */
/************************************************************************/

//////////////////////////////////////////////////////////////////////////
#include "./../core/MemAllocPool.h"
#include <list>
#include "Player.h"
#include "game_common.h"

//////////////////////////////////////////////////////////////////////////
enum ENUM_ROOM_ERROR
{
	ERE_ROOM_OK                  = 0,
	ERE_ROOM_UNKNOWN             = 100,
	ERE_ROOM_CAN_NOT_CREATED     = 101,       //�޷���������
	ERE_ROOM_CAN_NOT_CREATED_WITHOUTPAY,      //���ڽ�Ҳ����޷���������
	ERE_ROOM_IS_NOT_EXIST,                    //���䲻����
	ERE_ROOM_PLAYER_OUT_ROOM,                 //��Ҳ����κη�����
	ERE_ROOM_PLAYER_IN_HERE,                  //��ұ����ʹ����ڸ÷�����
	ERE_ROOM_PLAYER_IN_OTHER_ROOM,            //���������������
	ERE_ROOM_IN_GAME,                         //���䵱ǰ״̬Ϊ��Ϸ��״̬
	ERE_ROOM_IS_FULL,                         //��������
	ERE_ROOM_SOMEPLAYERINHERE,                //����������
	ERE_ROOM_PASSWORD_TYPE,                   //�����ʽ
	ERE_ROOM_WRONG_PASSWORD,                  //�������
	ERE_ROOM_LIMITRIGHT,                      //����Ȩ������
	ERE_ROOM_WRONG_STATUS,                    //��������״̬
};

//////////////////////////////////////////////////////////////////////////
enum ENUM_GAME_ROOM_TYPE
{
	EGRT_NONE = 0,          //�����Ͳ�����

	EGRT_TURN_BANKER,       //����ׯ��
	EGRT_FIGHT_BANKER,      //��ׯ��
	EGRT_NONE_BANKER,       //��ׯ��

	//////////////////////////////////////////////////////////////////////////
	EGRT_TURN_BANKER_DAY,   //
	EGRT_TURN_BANKER_WEEK,  //
	EGRT_TURN_BANKER_MONTH, //

	EGRT_FIGHT_BANKER_DAY,
	EGRT_FIGHT_BANKER_WEEK,
	EGRT_FIGHT_BANKER_MONTH,

	EGRT_NONE_BANKER_DAY,
	EGRT_NONE_BANKER_WEEK,
	EGRT_NONE_BANKER_MONTH,

	EGRT_COUNT,
};

//////////////////////////////////////////////////////////////////////////
enum ENUM_ROOM_STATUS
{
	ERS_NONE = 0,
	ERS_NOGAME,            //
	ERS_INGAME,            //��״̬�£����䲻�ɽ�ɢ��������뿪
};

struct BASE_ROOM
{
	//////////////////////////////////////////////////////////////////////////
	char           _OWNER_KEY[MAX_KEY_COUNT];            //���䴴���ߵ�KEY ID
	char           _PASSWORD[MAX_PASSWORD_COUNT];        //����Ϊ0���������룬�������룬����Ҫ������֤��������

	//////////////////////////////////////////////////////////////////////////
	const int      _ROOM_ID_RANDFLAG;                    //6λ��ֵ���������

	unsigned short _ROOM_ID;                             //����uid ��Χ0~MAX_ROOM_LIMIT
	short          _OWNER_UID;                           //���䴴����uid


	unsigned char  _MAX_ROUND;                           //������                     
	unsigned char  _BANKER_TYPE;                         //�淨���� ENUM_GAME_ROOM_TYPE


	//////////////////////////////////////////////////////////////////////////
	unsigned char  _current_round;                       //��ǰ����

	time_t         _start_time;                          //���䴴��ʱ��
	time_t         _agree_disband;                       //�����ɢ�ȴ�ͬ���ʱ��

	//////////////////////////////////////////////////////////////////////////
	ENUM_GAME_ROOM_TYPE   _room_type;           
	ENUM_ROOM_STATUS      _room_status;

	//////////////////////////////////////////////////////////////////////////
	BASE_PLAYER*          _Players[MAX_PLAYER_IN_ROOM];                     //
	unsigned char         _AgreeDisband[MAX_PLAYER_IN_ROOM];                //0:��ʼ״̬ 1:ͬ�� 2��ͬ��

	BASE_ROOM();
	void initData();

	//////////////////////////////////////////////////////////////////////////
	bool isNeedPassword();

	//////////////////////////////////////////////////////////////////////////
	//������Աȫ���˳�
	ENUM_ROOM_ERROR playersAllLeave();

	//���䱻��ɢ
	ENUM_ROOM_ERROR disband();

	//////////////////////////////////////////////////////////////////////////
	//0:�������� 1:������һ����ͬ����û�в���ͬ 2:������һ������ͬ
	int  checkPlayerAgreeToLeave();

	//
	void clearAgreeToLeave();
	int  getCurrentPlayersCount();
	void getPlayersInRoom(PLAYER_LIST& _playerList);

	//////////////////////////////////////////////////////////////////////////
	template<class T>
	void brodcast(T msg)
	{
		for( int i=0; i<MAX_PLAYER_IN_ROOM; i++ )
		{
			BASE_PLAYER* _player = _Players[i];
			if( _player != NULL && _player->_CLIENT )
			{
				SEND_MSG(msg, _player->_CLIENT);
			}
		}
	}
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

	friend class ROOM_LOCK;

public:
	static GameRooms* Instance()
	{
		static GameRooms _rooms;
		return &_rooms;
	}

	BASE_ROOM* get_room(unsigned short room_id);

	//�������� 0:�ɹ��������� 1:��Ҳ����޷��������� 2:���������޷��������� 3:�����������޷���������       >0xFF:����ԭ���޷���������
	ENUM_ROOM_ERROR createRoom(BASE_PLAYER* _player, BASE_ROOM*& _room);
	//���뷿�� 0:�ɹ����뷿�� 1:�����������޷����뷿�� 2:���������޷����뷿�� 3:�Ѿ��ڷ����ڣ��޷������·��� 0xF:���䲻�����޷����� >0xFF:����ԭ���޷����뷿��
	ENUM_ROOM_ERROR enterRoom(unsigned short _room_id, BASE_PLAYER* _player, const char* _password, BASE_ROOM*& _room);
	//�뿪���� 0:�ɹ��뿪���� 1:��Ϸ���޷��뿪���� >0xF:����ԭ���޷��뿪���� 0xF:���䲻�����޷��뿪 >0xFF:����ԭ���޷��뿪
	ENUM_ROOM_ERROR leaveRoom(BASE_PLAYER* _player, BASE_ROOM*& _room);
	//ȫ���뿪���� 0:�ɹ���ɢ���� 1:���˲�ͬ���ɢ���� 2:����״̬��������ɢ���� 0xF:���䲻�����޷���ɢ >0xFF:����ԭ���޷���ɢ����
	ENUM_ROOM_ERROR leaveGameRoom(bool _agree, BASE_PLAYER* _player, BASE_ROOM*& _room);
	//��ɢ���� 0:�ɹ���ɢ���� 1:����������Ϸ�У����ɽ�ɢ����
	ENUM_ROOM_ERROR disbandRoomByOwner(short _room_id, BASE_PLAYER* _player);

	//////////////////////////////////////////////////////////////////////////
	void updateRooms();

	void getRoomsByOwner(const unsigned short _player_id, ROOM_LIST& _room_list);
};

#endif