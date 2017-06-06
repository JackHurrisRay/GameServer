#include "comClient.h"
#include <WinSock2.h>

#include "./../core/JackBase64.h"
#include "./../game/game_common.h"
#include "./../game/Player.h"

#define JSON_COM_SERVER      "COM SERVER"
#define JSON_PROTOCAL        "protocal"
#define JSON_CONTENT         "content"
#define JSON_CONTENT_ID      "content_id"
#define JSON_STATUS          "status"
#define JSON_PLAYER_ID       "player_id"
#define JSON_PLAYER_DIAMOND  "player_diamond"

#define CONTENT_ID         "1493699600774844"

#define MAX_COM_BUFF       1024

//////////////////////////////////////////////////////////////////////////
enum ENUM_MSG_TYPE
{
	////
	ENUM_C2S_HEART = 2010,
	ENUM_C2S_CONNECT = 1000,
	ENUM_C2S_REQUEST_GOLD = 1001,
	ENUM_C2S_COST_GOLD = 1002,
	ENUM_C2S_RESULT_RECORD = 1010,

	////
	ENUM_S2C_HEART = 2015,
	ENUM_S2C_CONNECT = 2000,
	ENUM_S2C_REQUEST_GOLD = 2001,
	ENUM_S2C_COST_GOLD = 2002,
	ENUM_S2C_RESULT_RECORD = 1011,
};

//////////////////////////////////////////////////////////////////////////
comClient* comClient::Instance()
{
	static comClient client;
	return &client;
}

comClient::comClient(void):
	_socket(0),
	_isconn(false),
	_exit(false)
{
	
}


comClient::~comClient(void)
{

}

DWORD WINAPI thread_comWorker( LPVOID lpParam )
{
	unsigned long long _timecheck = GetTickCount64();

	while(!comClient::Instance()->_exit)
	{
		if( !comClient::Instance()->isConn() )
		{
			GAME_LOG("try to reconnect to ComServer...", true);

			if( comClient::Instance()->conn() && comClient::Instance()->comCheck() )
			{
				comClient::Instance()->_isconn = true;
				GAME_LOG("reconnect success!", true);
			}
			else
			{
				GAME_LOG("reconnect failed! retry again...", true);
			}
		}
		else
		{
			const unsigned long long _currenttime = GetTickCount64();

			if( _currenttime - _timecheck > 5000 )
			{
				comClient::Instance()->request_HEART();
				_timecheck = _currenttime;
			}

		}

		Sleep(1000);
	}

	CloseHandle(comClient::Instance()->_threadEvent);

	comClient::Instance()->closeSocket();

	return 0;
}

void comClient::closeSocket()
{
	if( _socket )
	{
		closesocket(_socket);
		_socket = 0;
	}
}

void comClient::start()
{
	_threadEvent = CreateEvent(0,0,0,0);

	HANDLE hMainThread = CreateThread(NULL, 0, thread_comWorker, _threadEvent, 0, 0);
	CloseHandle(hMainThread);
}

void comClient::end()
{
	_exit = true;
}

bool comClient::SendAndRecvData(Json::Value& _send, std::function<void(Json::Value& _recv)> _callback_recv)
{
	Json::FastWriter _writer;

	std::string _root_data = _writer.write(_send);

	std::string _buff = JackBase64::base64_encode((const unsigned char*)_root_data.c_str(), _root_data.size());
	send(_socket, _buff.c_str(), _buff.size(), 0);

	char recvBuff[MAX_COM_BUFF];
	memset(recvBuff, 0, sizeof(MAX_COM_BUFF));

	int _rev = recv(_socket, recvBuff, MAX_COM_BUFF, 0);

	bool _check = false;
	if( _rev > 0 )
	{
		recvBuff[_rev] = 0;

		std::string _jsonValue =
			JackBase64::base64_decode(recvBuff);

		Json::Value  _root;
		Json::Reader _reader;

		if( _reader.parse(_jsonValue.c_str(), _root) && !_root["protocal"].isNull() && 
			_root["protocal"].asUInt64() < 5000
			)
		{

			if( _root["protocal"].asUInt64() != 2015 )
			{
				GAME_LOG("com server recv:"<<_jsonValue.c_str(), true);
			}

			if( _callback_recv )
			{
				_callback_recv(_root);
				_check = true;
			}
		}
		else
		{
			_isconn = false;
			closesocket(_socket);
			_socket = 0;	
		}
	}
	else
	{
		//close socket
		_isconn = false;
		closesocket(_socket);
		_socket = 0;		
	}

	return _check;
}

bool comClient::isConn()
{
	return _isconn;
}

bool comClient::conn()
{
	bool _check = false;
	_socket = socket(AF_INET, SOCK_STREAM, 0);

	const int _port =
		JackBase64::GAME_CONFIG::Instance()->_comServerPort;

	const std::string _ip =
		JackBase64::GAME_CONFIG::Instance()->_comServerAddress;

	GAME_LOG("try to conn to >>"<<_ip<<":"<<_port, true);

	SOCKADDR_IN addr;
	addr.sin_addr.S_un.S_addr = inet_addr(_ip.c_str());
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);

	int ret = connect(_socket, (SOCKADDR*)&addr, sizeof(SOCKADDR));
	const int error_code = GetLastError();

	_check = (ret == 0);

	if(!_check)
	{
		GAME_LOG("tcp error code:"<<error_code, true);
		closesocket(_socket);
		_socket = 0;
	}

	return _check;
}

bool comClient::comCheck()
{
	_lock.lock();

	Json::Value _root;
	_root[JSON_PROTOCAL]   = ENUM_MSG_TYPE::ENUM_C2S_CONNECT;
	_root[JSON_COM_SERVER] = "Jack.L's Server";
	_root[JSON_CONTENT_ID] = CONTENT_ID;

	bool _check = 
	SendAndRecvData(_root,
		[](Json::Value& _recv)
	    {
			const int _protocal = _recv[JSON_PROTOCAL].asUInt64();
			const int _status   = _recv[JSON_STATUS].asUInt64();

			if( _protocal == ENUM_MSG_TYPE::ENUM_S2C_CONNECT && _status == 0 )
			{
				//////////////////////////////////////////////////////////////////////////
				GAME_LOG("connect to com Server", true);
			}
	    }
		);

	//////////////////////////////////////////////////////////////////////////
	_lock.unlock();

	if( !_check )
	{
		closesocket(_socket);
		_socket = 0;
	}

	return _check;
}

void comClient::request_HEART()
{
	if( !_isconn )
	{
		return;
	}

	_lock.lock();

	Json::Value _root;
	_root[JSON_PROTOCAL]   = ENUM_MSG_TYPE::ENUM_C2S_HEART;

	struct A
	{
		void recv(Json::Value _recv)
		{
			const int _protocal = _recv[JSON_PROTOCAL].asUInt64();
			const int _status   = _recv[JSON_STATUS].asUInt64();

			if( _protocal == ENUM_MSG_TYPE::ENUM_S2C_HEART )
			{
				//////////////////////////////////////////////////////////////////////////
			}
		}
	};

	A a;

	bool _check = 
		SendAndRecvData(_root,
		std::bind(&A::recv, a, std::placeholders::_1)
		);

	_lock.unlock();
}

void comClient::request_GOLD(BASE_PLAYER* _player, FUNC_REQUEST_GOLD callback)
{
	if( !_isconn )
	{
		return;
	}

	_lock.lock();

	Json::Value _root;
	_root[JSON_PROTOCAL]   = ENUM_MSG_TYPE::ENUM_C2S_REQUEST_GOLD;
	_root[JSON_CONTENT_ID] = CONTENT_ID;
	_root[JSON_PLAYER_ID]  = _player->_KEY;

	struct A
	{
		BASE_PLAYER* player;
		FUNC_REQUEST_GOLD func;
		void recv(Json::Value _recv)
		{
			const int _protocal = _recv[JSON_PROTOCAL].asUInt64();
			const int _status   = _recv[JSON_STATUS].asUInt64();

			if( _protocal == ENUM_MSG_TYPE::ENUM_S2C_REQUEST_GOLD && _status == 0 )
			{
				//////////////////////////////////////////////////////////////////////////
				if( func )
				{
					func(player, _recv[JSON_PLAYER_DIAMOND].asInt64());
				}
			}
		}
	};

	A a;
	a.player = _player;
	a.func   = callback;
		
	bool _check = 
		SendAndRecvData(_root,
		std::bind(&A::recv, a, std::placeholders::_1)
	);

	_lock.unlock();
}

void comClient::request_GOLD_Cost(unsigned long long _gold_cost, BASE_PLAYER* _player, FUNC_REQUEST_GOLD callback)
{
	if( !_isconn )
	{
		return;
	}

	_lock.lock();

	Json::Value _root;
	_root[JSON_PROTOCAL]   = ENUM_MSG_TYPE::ENUM_C2S_COST_GOLD;
	_root[JSON_CONTENT_ID] = CONTENT_ID;
	_root[JSON_PLAYER_ID]  = _player->_KEY;
	_root[JSON_PLAYER_DIAMOND] = _gold_cost;

	struct A
	{
		BASE_PLAYER* player;
		FUNC_REQUEST_GOLD func;
		void recv(Json::Value _recv)
		{
			const int _protocal = _recv[JSON_PROTOCAL].asUInt64();
			const int _status   = _recv[JSON_STATUS].asUInt64();

			if( _protocal == ENUM_MSG_TYPE::ENUM_S2C_COST_GOLD && _status >= 0 )
			{
				//////////////////////////////////////////////////////////////////////////
				if( func )
				{
					func(player, _recv[JSON_PLAYER_DIAMOND].asInt64());
				}
			}
		}
	};

	A a;
	a.player = _player;
	a.func   = callback;

	bool _check = 
		SendAndRecvData(_root,
		std::bind(&A::recv, a, std::placeholders::_1)
		);

	_lock.unlock();
}

void comClient::request_GAME_Record(std::string _recordStr)
{
	if( !_isconn )
	{
		return;
	}

	_lock.lock();

	Json::Value _root;
	_root[JSON_PROTOCAL]   = ENUM_MSG_TYPE::ENUM_C2S_RESULT_RECORD;
	_root["result"]  = _recordStr;

	struct A
	{
		void recv(Json::Value _recv)
		{
			const int _protocal = _recv[JSON_PROTOCAL].asUInt64();
			const int _status   = _recv[JSON_STATUS].asInt();

			if( _protocal == ENUM_MSG_TYPE::ENUM_S2C_RESULT_RECORD && _status == 0 )
			{
				//////////////////////////////////////////////////////////////////////////
				GAME_LOG("upload game record successfully", true);
			}
		}
	};

	A a;

	bool _check = 
		SendAndRecvData(_root,
		std::bind(&A::recv, a, std::placeholders::_1)
		);

	_lock.unlock();
}

