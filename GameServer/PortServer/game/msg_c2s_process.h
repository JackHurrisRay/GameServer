#ifndef _MSG_C2S_PROCESS_H
#define _MSG_C2S_PROCESS_H

namespace Json{
	class Value;
};

struct BASE_OBJECT;

#define NET_CALLBACK(X) bool NET_CALLBACK_##X(Json::Value& data, BASE_OBJECT* client)

class FACTORY_BEGIN
{
public:
	FACTORY_BEGIN();
};

//////////////////////////////////////////////////////////////////////////
extern int CHECK_PLAYER_FIT(BASE_OBJECT* client);

#define CHECK_MSG_PARAM(X) \
	if(!CHECK_PARAM<X>(data)){return false;}

#define  CHECK_MSG_LOGIN(X) \
	if( CHECK_PLAYER_FIT(X) != PROTOCAL_OK ){return true;}

#define  GET_PLAYER \
	BASE_PLAYER* player = Players::Instance()->get_player(client->_UID);

//////////////////////////////////////////////////////////////////////////
NET_CALLBACK(C2S_HEART);

NET_CALLBACK(C2S_LOGIN);
NET_CALLBACK(C2S_GETPLAYERDATA);
NET_CALLBACK(C2S_CREATE_ROOM);
NET_CALLBACK(C2S_ENTER_ROOM);
NET_CALLBACK(C2S_LEAVE_ROOM);
NET_CALLBACK(C2S_DISBAND_ROOM_BY_OWNER);
NET_CALLBACK(C2S_REQUEST_ROOMLIST);
NET_CALLBACK(C2S_PAY_VIP);

//////////////////////////////////////////////////////////////////////////
NET_CALLBACK(C2S_NEXT_AROUND);
NET_CALLBACK(C2S_APPLICATE_LEAVE);

NET_CALLBACK(C2S_READYGAME);
NET_CALLBACK(C2S_CANCELREADY);
NET_CALLBACK(C2S_FIGHT_ZHUANG);
NET_CALLBACK(C2S_ADD_DOUBLE);

#endif // !_MSG_C2S_PROCESS_H
