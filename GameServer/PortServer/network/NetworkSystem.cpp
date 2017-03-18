#include "NetworkSystem.h"
//////////////////////////////////////////////////////////////////////////
/******************************************************************  
* per_handle 数据  
*******************************************************************/   
#include <iostream>
#include "./../core/WebSocketSystem.h"
#include "./../core/JackBase64.h"
#include "./../core/ClientObjectSystem.h"

//////////////////////////////////////////////////////////////////////////
NetworkSystem* _NETWORK_SYS = NULL;
NetworkSystem* NetworkSystem::Instance()
{
	if( _NETWORK_SYS == NULL )
	{
		_NETWORK_SYS = new NetworkSystem;
	}

	return _NETWORK_SYS;
}

void NetworkSystem::Release()
{
	delete _NETWORK_SYS;
	_NETWORK_SYS = NULL;
}

//////////////////////////////////////////////////////////////////////////
NetworkSystem::NetworkSystem(void):
	m_exitCommand(false)
{
	WSADATA wsaData;
	int _hr = WSAStartup(MAKEWORD(2,2), &wsaData);

	ClientObjectSystem::Instance();
}


NetworkSystem::~NetworkSystem(void)
{
	ClientObjectSystem::Release();
}

void NetworkSystem::cmdEnd()
{
	closesocket(m_listener);
	CloseHandle(m_hIocp);
	m_exitCommand = true;
}

void NetworkSystem::start(int port)
{
	cout << "server ready......" << endl; 

	//////////////////////////////////////////////////////////////////////////
	m_hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	m_listener = socket(AF_INET, SOCK_STREAM, 0);


	//////////////////////////////////////////////////////////////////////////
	SOCKADDR_IN addr;

	addr.sin_family = AF_INET;
	addr.sin_port   = htons(port);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind( m_listener, (sockaddr *)&addr, sizeof(addr) );
	listen( m_listener, 5 );


	//cpu个数*2+2 通常为最佳线程数  
	SYSTEM_INFO sysInfo;  
	GetSystemInfo(&sysInfo);  
	int ThreadNum=sysInfo.dwNumberOfProcessors*2+2;  
	HANDLE hEvent;

	for(int i=0;i<ThreadNum;i++)  
	{  
		HANDLE hThread;  
		
		hEvent = CreateEvent(0,0,0,0);
		hThread = CreateThread(NULL, 0, thread_worker, hEvent, 0, 0);  
		CloseHandle(hThread);

		m_handleList.push_back(hEvent);
	}  

	////	
	hEvent = CreateEvent(0,0,0,0);
	HANDLE hMainThread = CreateThread(NULL, 0, thread_main, hEvent, 0, 0);
	CloseHandle(hMainThread);

	m_handleList.push_back(hEvent);

	////	
	hEvent = CreateEvent(0,0,0,0);
	HANDLE hLogicThread = CreateThread(NULL, 0, thread_logic, hEvent, 0, 0);
	CloseHandle(hLogicThread);

	m_handleList.push_back(hEvent);

	//end server
	while (m_handleList.size()>0)
	{
		Sleep(100);
	}

	cout << "server exit......" << endl; 

	Sleep(600);
}

void NetworkSystem::close_threadHandle(HANDLE _h)
{
	lock(ELT_WORKER_EXIT);

	m_handleList.remove(_h);
	CloseHandle(_h);

	unlock(ELT_WORKER_EXIT);
}

void NetworkSystem::listen_connected()
{
	for (;;)
	{
		if( m_exitCommand )
		{
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		SOCKADDR_IN saRemote;   
		int nRemoteLen = sizeof( saRemote );   
		SOCKET sRemote = accept( m_listener, (sockaddr *)&saRemote, &nRemoteLen );  
		cout << "client join......" << endl; 

		PPER_HANDLE_DATA pPerHandle =(PPER_HANDLE_DATA)::GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));  
		if( pPerHandle == NULL )   
		{   
			break;   
		}  

		pPerHandle->s = sRemote;   
		memcpy( &pPerHandle->addr, &saRemote, nRemoteLen );  

		//关联iocp和接收socket  
		CreateIoCompletionPort( ( HANDLE)pPerHandle->s, m_hIocp, (DWORD)pPerHandle, 0 );  
		PPER_IO_DATA pIoData =(PPER_IO_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_DATA));  

		if( pIoData == NULL )   
		{   
			break;   
		}  

		//////////////////////////////////////////////////////////////////////////
		ClientObjectSystem::Instance()->addBaseObject(pPerHandle, pIoData);

		//////////////////////////////////////////////////////////////////////////
		pIoData->nOperationType = OP_ACCEPT;   
		WSABUF buf;   
		buf.buf = pIoData->buf;   
		buf.len = BUFFER_SIZE;   

		DWORD dwRecv  = 0;   
		DWORD dwFlags = 0;//注意保证dwFlags为0，否则会出错  
		
		//压缩数据到缓冲区  
		memset(buf.buf, 0, BUFFER_SIZE);
		WSARecv( pPerHandle->s, &buf, 1, &dwRecv, &dwFlags, &pIoData->ol, NULL );  
	}
}

void NetworkSystem::closeClientSocket( PPER_HANDLE_DATA pPerHandle, PPER_IO_DATA pPerIo )
{
	if( pPerHandle )
	{
		ClientObjectSystem::Instance()->delBaseObject(pPerHandle->s);

		closesocket( pPerHandle->s );   
		GlobalFree( pPerHandle );   
	}

	if( pPerIo )
	{
		GlobalFree( pPerIo );  
	}

	cout << "client closed......" << endl;   
}

void NetworkSystem::sendMSG( PPER_HANDLE_DATA pPerHandle, PPER_IO_DATA pPerIo, size_t _buffsize, ENUM_OP_TYPE _type )
{
	WSABUF buf;   
	buf.buf = pPerIo->buf;   
	buf.len = _buffsize;   
	pPerIo->nOperationType = _type;       

	//发送数据  
	DWORD dwSend  = 0;   
	DWORD dwFlags = 0;  
	WSASend(pPerHandle->s,&buf,1,&dwSend,dwFlags,&pPerIo->ol,NULL);  
}

void NetworkSystem::request_Recv( PPER_HANDLE_DATA pPerHandle, PPER_IO_DATA pPerIo, ENUM_OP_TYPE _type, size_t _size )
{
	pPerIo->nOperationType = _type;   
	WSABUF buf;   
	buf.buf = pPerIo->buf;   
	buf.len = _size;   

	DWORD dwRecv  = 0;   
	DWORD dwFlags = 0;//注意保证dwFlags为0，否则会出错  

	//压缩数据到缓冲区  
	memset(buf.buf, 0, BUFFER_SIZE);
	WSARecv( pPerHandle->s, &buf, 1, &dwRecv, &dwFlags, &pPerIo->ol, NULL );  
}

void NetworkSystem::work()
{
	HANDLE hIocp = m_hIocp;

	DWORD dwTrans = 0;   
	PPER_HANDLE_DATA pPerHandle;   
	PPER_IO_DATA     pPerIo;   

	while( TRUE )   
	{   
		// 在关联到此完成端口的所有套接字上等待I/O完成   
		BOOL bRet = GetQueuedCompletionStatus( hIocp, &dwTrans, (LPDWORD)&pPerHandle, (LPOVERLAPPED*)&pPerIo, WSA_INFINITE );   

		if( m_exitCommand )
		{
			break;
		}

		if( !bRet )     // 发生错误   
		{   
			closeClientSocket( pPerHandle, pPerIo );
			cout << "error" << endl;   
			continue;   
		}  
		else
		// 套接字被对方关闭   
		if( dwTrans == 0 && ( pPerIo->nOperationType == OP_READ_HEADER || pPerIo->nOperationType == OP_READ_DATA || pPerIo->nOperationType== OP_WRITE ) )   
		{   
			closeClientSocket( pPerHandle, pPerIo );
			continue;   
		}  
		if( dwTrans >= 2048 )
		{
			//错误数据
			closeClientSocket( pPerHandle, pPerIo );
			continue;   
		}

		//读写请求压入事务线程中去
		switch ( pPerIo->nOperationType )
		{
		case OP_READ_HEADER:
			{
				//收到数据包头 
				if( dwTrans == 4 && ClientObjectSystem::Instance()->readHeader(pPerHandle->s, pPerIo->buf) )
				{
					//

				}
				else
				{
					//
					closeClientSocket( pPerHandle, pPerIo );
				}

				break;
			}
		case OP_READ_DATA:
			{
				//收到数据包
				if( dwTrans > 0 && ClientObjectSystem::Instance()->readData(pPerHandle->s, pPerIo->buf, dwTrans ) )
				{
					//

				}
				else
				{
					//
					closeClientSocket( pPerHandle, pPerIo );
				}

				break;
			}
		case OP_WRITE:
			{
				request_Recv(pPerHandle, pPerIo);
				break;
			}
		case OP_ACCEPT:
			{
				//收到握手请求
				pPerIo->buf[dwTrans + 1] = '\0';  

				if( ClientObjectSystem::Instance()->checkHandsFromClient(pPerHandle->s, pPerIo->buf) )
				{
					//
					sendMSG(pPerHandle, pPerIo, strlen(pPerIo->buf), OP_CHECKHANDS);
				}
				else
				{
					//
					closeClientSocket( pPerHandle, pPerIo );
				}
				
				break;
			}
		case OP_CHECKHANDS:
			{
				//握手请求抵达
				pPerIo->buf[dwTrans + 1] = '\0';  

				if( ClientObjectSystem::Instance()->checkHandsSuccess(pPerHandle->s) )
				{
					//
					request_Recv( pPerHandle, pPerIo );
				}
				else
				{
					//
					closeClientSocket( pPerHandle, pPerIo );
				}

				break;
			}
		}
	}  
}

void NetworkSystem::running()
{
	while (true)
	{
		if( m_exitCommand )
		{
			break;
		}

		//////////////////////////////////////////////////////////////////////////
		ClientObjectSystem::Instance()->running();

	}
}

DWORD WINAPI thread_main( LPVOID lpParam )
{
	cout << "server main start......" << endl; 

	NetworkSystem* _network = NetworkSystem::Instance();

	_network->listen_connected();

	_network->close_threadHandle((HANDLE)lpParam);

	cout << "server main exit......" << endl; 

	return 0;
}

DWORD WINAPI thread_worker( LPVOID lpParam )
{
	cout << "server work start......" << endl; 
	const HANDLE _handle = lpParam;

	NetworkSystem* _network = NetworkSystem::Instance();
	_network->work();
	_network->close_threadHandle(_handle);

	cout << "server work exit......" << endl; 

	return 0;
}

DWORD WINAPI thread_logic( LPVOID lpParam )
{
	cout << "server logic start......" << endl; 

	NetworkSystem* _network = NetworkSystem::Instance();

	_network->running();

	_network->close_threadHandle((HANDLE)lpParam);

	cout << "server logic exit......" << endl; 

	return 0;
	return 0;
}
