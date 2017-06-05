#ifndef _COMCLIENT_H
#define _COMCLIENT_H

#include "./../core/MemAllocPool.h"
#include "json/json.h"

#include <functional>

struct BASE_PLAYER;

typedef std::function<void(BASE_PLAYER* _player, long long _gold)> FUNC_REQUEST_GOLD;

class comClient
{
protected:
	int    _socket;
	Locker _lock;
	bool   _isconn;

	HANDLE _threadEvent;
	bool   _exit;

	comClient(void);
	~comClient(void);

	bool conn();
	bool comCheck();

	bool SendAndRecvData(Json::Value& _send, std::function<void(Json::Value& _recv)> _callback_recv);
	void closeSocket();

	friend DWORD WINAPI thread_comWorker( LPVOID lpParam );

	void request_HEART();

public:

	static comClient* Instance();

	void start();
	void end();

	void request_GOLD(BASE_PLAYER* _player, FUNC_REQUEST_GOLD callback);
	void request_GOLD_Cost(unsigned long long _gold_cost, BASE_PLAYER* _player, FUNC_REQUEST_GOLD callback);
	void request_GAME_Record(std::string _recordStr);

	bool isConn();
};

#endif // !_COMCLIENT_HJ
