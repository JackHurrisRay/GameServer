#ifndef _GAME_COMMON_H
#define _GAME_COMMON_H

#include <iostream>
#include <list>
#include <vector>

#define MAX_PASSWORD_COUNT       8
#define MAX_NICKNAME_COUNT       32
#define MAX_KEY_COUNT            64

#define MAX_ROOM_LIMIT           4000
#define MAX_PLAYER_LIMIT         MAX_ROOM_LIMIT * 3 / 2

#define MAX_PLAYER_IN_ROOM       8

//////////////////////////////////////////////////////////////////////////
#define __LOG_FLAG true
#define GAME_LOG(X, _switch) if(__LOG_FLAG && _switch){std::cout << "******LOG:" << X << "******" << std::endl;fflush(stdout);}

typedef std::list<unsigned short> UINT16_LIST;
typedef std::list<unsigned int>   UINT32_LIST;

#endif