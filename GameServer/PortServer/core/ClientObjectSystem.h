#ifndef _CLIENTOBJECTSYSTEM_H
#define _CLIENTOBJECTSYSTEM_H

#include "ClientObject.h"

class ClientObjectSystem
{
protected:

	Locker             m_lockForAccountMap;
	BASE_OBJECT_ALLOC  m_accountAlloc;
	ACCOUNT_MAP        m_accountMap;

	ClientObjectSystem(void);
	~ClientObjectSystem(void);

	bool objectMethod(BASE_OBJECT* _object, char* _buff);


public:
	static ClientObjectSystem* Instance()
	{
		static ClientObjectSystem _cos;
		return &_cos;
	}

	void addBaseObject( PER_HANDLE_DATA* pPerHandle, PER_IO_DATA* pPerIo );
	void delBaseObject(__BRIDGE);

	bool checkHandsFromClient(__BRIDGE, char*);
	bool checkHandsSuccess(__BRIDGE);

	bool readHeader(__BRIDGE, char*);
	bool readData(__BRIDGE, char*, size_t _len);

	void PushMSGFromClient(__BRIDGE, char*);
	void PopMsgToClient(__BRIDGE, char*);

	void running();
};


#endif

