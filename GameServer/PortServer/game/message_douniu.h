#ifndef _MESSAGE_DOUNIU_H
#define _MESSAGE_DOUNIU_H

#include "message.h"

struct  MSG_C2S_STARTGAME:
	public BASE_PROTOCAL_MSG
{
	MSG_C2S_STARTGAME():BASE_PROTOCAL_MSG(ENUM_GAME_PROTOCAL::EGP_C2S_STARTGAME)
	{

	}
};



#endif