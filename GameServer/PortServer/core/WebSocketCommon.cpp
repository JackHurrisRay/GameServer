#include "WebSocketCommon.h"
#include "JackBase64.h"
#include <string>
#include <map>
#include <iostream>


//////////////////////////////////////////////////////////////////////////
const char* KEY_WEBSOCKET_HANDCHECK[EWH_COUNT] =	
{
	"Host",
	"Connection",
	"Pragma",
	"Cache-Control",
	"Upgrade",
	"Origin",
	"Sec-WebSocket-Version",
	"User-Agent",
	"Accept-Encoding",
	"Accept-Language",
	"Sec-WebSocket-Key",
	"Sec-WebSocket-Extensions",
	"Sec-WebSocket-Origin",
};

ENUM_WEBSOCKET_HANDSCHECK check_enum_for_websocket_handcheck(const char* _key)
{
	int i=0;
	int _result = EWH_COUNT;

	for( ; i<EWH_COUNT;i++)
	{
		if( strcmpi(KEY_WEBSOCKET_HANDCHECK[i], _key) == 0 )
		{
			_result = i;
			break;
		}
		else
		{
			_result = EWH_COUNT;
		}
	}

	return (ENUM_WEBSOCKET_HANDSCHECK)_result;
}

std::string get_key_for_client_ask_handscheck(std::string _sec_key)
{
	//////////////////////////////////////////////////////////////////////////
	std::string _key_value = _sec_key;//JackBase64::base64_decode(_sec_key);

	//////////////////////////////////////////////////////////////////////////
	const char* _ex_str = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	_key_value += _ex_str;


	//////////////////////////////////////////////////////////////////////////
	SHA1 sha;
	sha.Reset();
	sha << _key_value.c_str();

	unsigned int iDigSet[5];
	memset(iDigSet, 0, sizeof(iDigSet));

	sha.Result(iDigSet);

	for (int i = 0; i < 5; i++)
	{
		iDigSet[i] = htonl(iDigSet[i]);
	}

	//////////////////////////////////////////////////////////////////////////
	std::string _result = JackBase64::base64_encode( reinterpret_cast<const unsigned char*>(iDigSet), 20 );

	return _result;
}
















