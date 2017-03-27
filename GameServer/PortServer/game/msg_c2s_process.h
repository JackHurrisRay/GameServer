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
NET_CALLBACK(C2S_LOGIN);
NET_CALLBACK(C2S_GETPLAYERDATA);
NET_CALLBACK(C2S_CREATE_ROOM);
NET_CALLBACK(C2S_ENTER_ROOM);
NET_CALLBACK(C2S_LEAVE_ROOM);
NET_CALLBACK(C2S_DISBAND_ROOM_BY_OWNER);
NET_CALLBACK(C2S_REQUEST_ROOMLIST);

#endif // !_MSG_C2S_PROCESS_H
