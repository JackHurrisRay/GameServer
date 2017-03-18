#include "WebSocketSystem.h"
#include <istream>
#include <sstream>
#include <iostream>
#include <string>
#include <time.h>

#include "JackBase64.h"

//////////////////////////////////////////////////////////////////////////
WebSocketSystem::WebSocketSystem(void)
{
}


WebSocketSystem::~WebSocketSystem(void)
{
}

int WebSocketSystem::trans_c2s(char* _buff)
{
	return 0;
}

void WebSocketSystem::trans_s2c(char* _buff)
{

}

int WebSocketSystem::checkHands_c2s(char* _buff, std::string& _output)
{
	//////////////////////////////////////////////////////////////////////////
	std::istringstream s(_buff);
	std::string request;

	std::getline(s, request);
	if (request[request.size()-1] == '\r') {
		request.erase(request.end()-1);
	} else {
		return -1;
	}

	//////////////////////////////////////////////////////////////////////////
	struct ACCEPT_DATA
	{
		std::string _value[EWH_COUNT];
	};

	std::string _secKeyAccept;
	ACCEPT_DATA _inData;

	//////////////////////////////////////////////////////////////////////////
	std::string header;
	std::string::size_type end;

	while (std::getline(s, header) && header != "\r") {
		if (header[header.size()-1] != '\r') {
			continue; //end
		} else {
			header.erase(header.end()-1);    //remove last char
		}

		end = header.find(": ",0);
		if (end != std::string::npos) {
			std::string key   = header.substr(0,end);
			std::string value = header.substr(end+2);

			////
			ENUM_WEBSOCKET_HANDSCHECK _enum_checkhands = check_enum_for_websocket_handcheck(key.c_str());

			if( _enum_checkhands == EWH_COUNT )
			{
				return -1;
			}

			_inData._value[(int)_enum_checkhands] = key;

			//////////////////////////////////////////////////////////////////////////
			switch (_enum_checkhands)
			{
			case EWH_HOST:
				break;
			case EWH_CONNECTION:
				break;
			case EWH_PRAGMA:
				break;
			case EWH_CACHE_CONTROL:
				break;
			case EWH_UPGRADE:
				break;
			case EWH_ORIGIN:
				break;
			case EWH_SEC_WEBSOCKET_VERSION:

				//必须等于“13”
				if( strcmpi(value.c_str(), "13") != 0 )
				{
					return -1;
				}

				break;
			case EWH_USER_AGENT:
				break;
			case EWH_ACCEPT_ENCODING:
				break;
			case EWH_ACCEPT_LANGUAGE:
				break;
			case EWH_SEC_WEBSOCKET_KEY:
				{
					//解析握手key
					std::string _decodeValue = value;
					
#ifdef _DEBUG
					//std::cout << "decode:" << _decodeValue << "  size:" << strlen(_decodeValue.c_str()) << std::endl;
#endif

					std::string _accept_result = get_key_for_client_ask_handscheck(_decodeValue);
					
#ifdef _DEBUG
					//std::cout << "accept:" << _accept_result << "  size:" << strlen(_accept_result.c_str()) << std::endl;
#endif

					//////////////////////////////////////////////////////////////////////////
					_secKeyAccept = _accept_result;

					break;
				}
			case EWH_SEC_WEBSOCKET_EXTENSIONS:
				break;
			case EWH_SEC_WEBSOCKET_ORIGIN:
				break;
			case EWH_COUNT:
				break;
			default:
				break;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
    /*
	time_t tt = time(NULL);//这句返回的只是一个时间cuo
	tm* t= localtime(&tt);

	char _strTime[256];

	sprintf(_strTime,
		"%d-%02d-%02d %02d:%02d:%02d\n", 
		t->tm_year + 1900,
		t->tm_mon + 1,
		t->tm_mday,
		t->tm_hour,
		t->tm_min,
		t->tm_sec);
	*/

	_output = "HTTP/1.1 101 Switching Protocols\r\n";
	_output += "Connection: Upgrade\r\n";
	_output += "Sec-WebSocket-Accept: ";
	_output += _secKeyAccept;
	_output += "\r\n";
	_output += "Access-Control-Allow-Credentials:ture\r\n";
	_output += "Access-Control-Alow-Headers:content-type\r\n";
	_output += "Upgrade: websocket\r\n\r\n";


#ifdef _DEBUG
	//std::cout << std::endl << _output << std::endl;
#endif 
	

	return 0;
}

void WebSocketSystem::checkHands_s2c(char* _buff)
{

}
