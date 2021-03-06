// PortServer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <crtdbg.h>
#include <iostream>

#include "network/NetworkSystem.h"

//////////////////////////////////////////////////////////////////////////
#include "game/PokeCard.h"
#include "game/game_common.h"
#include "common/JACK_LIST.h"
#include "core/JackBase64.h"

DWORD WINAPI thread_exit( LPVOID lpParam );
int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//////////////////////////////////////////////////////////////////////////
	/*
	CPokeCard cardObj;
	cardObj.Shuffle_cards();

	JACK_LIST<int> _TEST_LIST;
	for( int i=0; i<1000; i++ )
	{
		_TEST_LIST.push_back((int*)i);
	}

	_TEST_LIST.close_loop();
	_TEST_LIST.clear();
	*/

	//////////////////////////////////////////////////////////////////////////
	JackBase64::GAME_CONFIG* _config = JackBase64::GAME_CONFIG::Instance();
	const int _server_port = _config->_SERVER_PORT;


	//////////////////////////////////////////////////////////////////////////
	NetworkSystem::Instance();

	HANDLE hMainThread = CreateThread(NULL, 0, thread_exit, NetworkSystem::Instance(), 0, 0);
	CloseHandle(hMainThread);

	NetworkSystem::Instance()->start(_server_port);

	NetworkSystem::Release();

	//////////////////////////////////////////////////////////////////////////
	int i;
	GAME_LOG( "press any key to exit..." << std::endl, true );
	std::cin >> i;

	return 0;
}

//////////////////////////////////////////////////////////////////////////
#include "game/Player.h"

DWORD WINAPI thread_exit( LPVOID lpParam )
{
	char _command[64];

	while (true)
	{
		memset(_command, 0, sizeof(_command));

		fflush(stdin);
		std::cin >> _command;

		if(strcmpi(_command, "exit") == 0)
		{
			NetworkSystem::Instance()->cmdEnd();
			break;
		}
		else if( strcmpi(_command, "playercount") == 0 )
		{
			GAME_LOG( "playercount:" << Players::Instance()->getPlayerCountOnLine() << std::endl, true );
		}
		else if( strcmpi(_command, "clientcount") == 0 )
		{
			GAME_LOG( "clientcount:" << _ALLOC_PER_IO_DATA.getUsedCound() << std::endl, true );
		}

	}

	return 0;
}