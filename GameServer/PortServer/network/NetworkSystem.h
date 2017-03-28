#pragma once
#ifndef _NETWORKSYSTEM_H
#define _NETWORKSYSTEM_H

#define MAX_CLIENT_CONN 8000
#include "./../core/MemAllocPool.h"

#include <WinSock2.h>
#include <list>
using namespace std;

typedef std::list<HANDLE> HANDLE_LIST;

//////////////////////////////////////////////////////////////////////////
struct PER_HANDLE_DATA   
{   
	SOCKET           s;                   // 对应的套接字句柄   
	sockaddr_in      addr;                // 对方的地址  
};

typedef PER_HANDLE_DATA* PPER_HANDLE_DATA;  
/******************************************************************  
* per_io 数据  
*******************************************************************/   
enum  ENUM_OP_TYPE
{
	OP_NONE   = 0,
	OP_ACCEPT = 1,
	OP_CHECKHANDS,
	OP_READ_HEADER,     //header
	OP_READ_HEADER_EX,
	OP_READ_DATA,       //data
	OP_WRITE,
	OP_BRODCAST,        //
};

struct PER_IO_DATA   
{   
	OVERLAPPED           ol;                   // 重叠结构   
	char                 buf[BUFFER_SIZE];     // 数据缓冲区   
	ENUM_OP_TYPE         nOperationType;       // 操作类型  
};
typedef PER_IO_DATA* PPER_IO_DATA;  

typedef MemAllocPool<PER_HANDLE_DATA> ALLOC_PER_HANDLE_DATA;
typedef MemAllocPool<PER_IO_DATA>     ALLOC_PER_IO_DATA;

extern ALLOC_PER_HANDLE_DATA _ALLOC_PER_HANDLE_DATA;
extern ALLOC_PER_IO_DATA     _ALLOC_PER_IO_DATA;
//////////////////////////////////////////////////////////////////////////
enum  ENUM_LOCKER_TYPE
{
	ELT_WORKER = 0,
	ELT_WORKER_EXIT,

	ELT_COUNT,
};

class BASE_LOCKER
{
protected:
	Locker m_lock[ELT_COUNT];

	void lock(ENUM_LOCKER_TYPE _t){m_lock[_t].lock();};
	void unlock(ENUM_LOCKER_TYPE _t){m_lock[_t].unlock();};
};

//////////////////////////////////////////////////////////////////////////
class NetworkSystem:
public	BASE_LOCKER
{
protected:

    SOCKET        m_listener;
	HANDLE        m_hIocp;	
	HANDLE_LIST   m_handleList;

	NetworkSystem(void);
	~NetworkSystem(void);

	bool m_exitCommand;

	friend DWORD WINAPI thread_worker( LPVOID lpParam );
	friend DWORD WINAPI thread_main( LPVOID lpParam );
	friend DWORD WINAPI thread_logic( LPVOID lpParam );

	void closeClientSocket( PPER_HANDLE_DATA& pPerHandle, PPER_IO_DATA& pPerIo );

	void listen_connected();
	void work();
	void running();

	void close_threadHandle(HANDLE _h);

public:

	static NetworkSystem* Instance();
	static void Release();

	void start(int port);
	void cmdEnd();

	void sendMSG( PPER_HANDLE_DATA pPerHandle, PPER_IO_DATA pPerIo, size_t _buffsize, ENUM_OP_TYPE _type );
	void request_Recv( PPER_HANDLE_DATA pPerHandle, PPER_IO_DATA pPerIo, ENUM_OP_TYPE _type = OP_READ_HEADER, size_t _size = 2 );
};

#endif