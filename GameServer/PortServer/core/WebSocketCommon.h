#pragma once

#ifndef _WEBSOCKETCOMMON_H
#define _WEBSOCKETCOMMON_H

#include <string>

//////////////////////////////////////////////////////////////////////////
//include extern
#include "./../extern/sha1.h"

//////////////////////////////////////////////////////////////////////////
//include windows
#define WINDOWS

#ifdef WINDOWS
#include <WinSock2.h>
#include <Windows.h>
typedef SOCKET __BRIDGE;
typedef SOCKET __LD;        //on linux this must define int

#endif // WINDOWS

#ifdef LINUX
typedef int    __BRIDGE;
typedef int    __LD;
#endif // LINUX

//////////////////////////////////////////////////////////////////////////

enum ENUM_WEBSOCKET_HANDSCHECK
{
	EWH_HOST = 0,
	EWH_CONNECTION,
	EWH_PRAGMA,
	EWH_CACHE_CONTROL,
	EWH_UPGRADE,
	EWH_ORIGIN,
	EWH_SEC_WEBSOCKET_VERSION,
	EWH_USER_AGENT,
	EWH_ACCEPT_ENCODING,
	EWH_ACCEPT_LANGUAGE,
	EWH_SEC_WEBSOCKET_KEY,
	EWH_SEC_WEBSOCKET_EXTENSIONS,

	EWH_SEC_WEBSOCKET_ORIGIN,

	EWH_COUNT,
};

ENUM_WEBSOCKET_HANDSCHECK check_enum_for_websocket_handcheck(const char* _key);
std::string get_key_for_client_ask_handscheck(std::string _sec_key);

#endif


