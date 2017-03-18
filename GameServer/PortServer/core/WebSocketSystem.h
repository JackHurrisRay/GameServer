#ifndef _WEBSOCKETSYSTEM_H
#define _WEBSOCKETSYSTEM_H

#include "WebSocketCommon.h"

extern const char* KEY_WEBSOCKET_HANDCHECK[EWH_COUNT];

class WebSocketSystem
{
protected:
	WebSocketSystem(void);
	~WebSocketSystem(void);


public:

	static WebSocketSystem* Instance()
	{
		static WebSocketSystem _sys;
		return &_sys;
	}

	int checkHands_c2s(char* _buff, std::string& _output);
	void checkHands_s2c(char* _buff);

	int trans_c2s(char* _buff);
	void trans_s2c(char* _buff);
};

#endif