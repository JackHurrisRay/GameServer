// PortServer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <crtdbg.h>
#include <iostream>

#include "network/NetworkSystem.h"

//////////////////////////////////////////////////////////////////////////
#include "game/PokeCard.h"
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


	//////////////////////////////////////////////////////////////////////////
	NetworkSystem::Instance();

	HANDLE hMainThread = CreateThread(NULL, 0, thread_exit, NetworkSystem::Instance(), 0, 0);
	CloseHandle(hMainThread);

	NetworkSystem::Instance()->start(1234);

	NetworkSystem::Release();

	//////////////////////////////////////////////////////////////////////////
	int i;
	std::cout << "press any key to exit..." << std::endl;
	std::cin >> i;

	return 0;
}

DWORD WINAPI thread_exit( LPVOID lpParam )
{
	char _command[64];

	while (true)
	{
		memset(_command, 0, sizeof(_command));
		std::cin >> _command;

		//std::cout << "out:" << _command << std::endl;

		if(strcmpi(_command, "exit") == 0)
		{
			NetworkSystem::Instance()->cmdEnd();
			break;
		}

	}

	return 0;
}