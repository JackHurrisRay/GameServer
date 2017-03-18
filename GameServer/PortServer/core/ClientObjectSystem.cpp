#include "ClientObjectSystem.h"
#include "WebSocketSystem.h"
#include "./../network/NetworkSystem.h"

/************************************************************************/
/* system                                                               */
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
ClientObjectSystem* _CLIENT_OBJ_SYS = NULL;
ClientObjectSystem* ClientObjectSystem::Instance()
{
	if( _CLIENT_OBJ_SYS == NULL )
	{
		_CLIENT_OBJ_SYS = new ClientObjectSystem;
	}

	return _CLIENT_OBJ_SYS;
}

void ClientObjectSystem::Release()
{
	delete _CLIENT_OBJ_SYS;
	_CLIENT_OBJ_SYS = NULL;
}

ClientObjectSystem::ClientObjectSystem(void):
	m_accountAlloc( NULL )
{
	m_accountAlloc = new BASE_OBJECT_ALLOC(MAX_CLIENT_CONN);

	return;
}


ClientObjectSystem::~ClientObjectSystem(void)
{
	delete m_accountAlloc;
}

void ClientObjectSystem::addBaseObject( PER_HANDLE_DATA* pPerHandle, PER_IO_DATA* pPerIo )
{
	m_lockForAccountMap.lock();

	//////////////////////////////////////////////////////////////////////////
	BASE_OBJECT* _obj = m_accountAlloc->createData();
	_obj->reset();

	_obj->_bridge_client = pPerHandle->s;
	_obj->_pPerHandle    = pPerHandle;
	_obj->_pPerIo        = pPerIo;

	//////////////////////////////////////////////////////////////////////////
	m_accountMap.insert(ACCOUNT_MAP::value_type(_obj->_bridge_client, _obj));

	m_lockForAccountMap.unlock();
}

void ClientObjectSystem::delBaseObject(__BRIDGE _client)
{
	m_lockForAccountMap.lock();

	ACCOUNT_MAP::iterator _cell = m_accountMap.find(_client);

	if( m_accountMap.end() != _cell )
	{
		//////////////////////////////////////////////////////////////////////////
		BASE_OBJECT* _obj = _cell->second;
		m_accountMap.erase(_client);

		//////////////////////////////////////////////////////////////////////////
		_obj->release();

		//////////////////////////////////////////////////////////////////////////
		m_accountAlloc->releaseData(_obj);
	}

	m_lockForAccountMap.unlock();
}

bool ClientObjectSystem::checkHandsFromClient(__BRIDGE _client, char* _buff_in)
{
	bool _status = false;

	m_lockForAccountMap.lock();
	ACCOUNT_MAP::iterator _cell = m_accountMap.find(_client);

	if( m_accountMap.end() != _cell )
	{
		BASE_OBJECT* _obj = _cell->second;

		std::string _output;

		if(_obj != NULL && WebSocketSystem::Instance()->checkHands_c2s(_buff_in, _output) == 0)
		{
			memset(_buff_in, 0, BUFFER_SIZE);
			strcpy(_buff_in,_output.c_str()); 

			_status = true;
		}
	}

	m_lockForAccountMap.unlock();

	return _status;
}

bool ClientObjectSystem::checkHandsSuccess(__BRIDGE _client)
{
	bool _status = false;

	m_lockForAccountMap.lock();
	ACCOUNT_MAP::iterator _cell = m_accountMap.find(_client);

	if( m_accountMap.end() != _cell )
	{
		BASE_OBJECT* _obj = _cell->second;

		std::string _output;

		if(_obj != NULL)
		{
			_obj->_bHandshake = true;
			_status = true;
		}
	}

	m_lockForAccountMap.unlock();

	return _status;
}

//////////////////////////////////////////////////////////////////////////
bool ClientObjectSystem::readHeader(__BRIDGE _client, char* _buff)
{
	bool _status = false;

	m_lockForAccountMap.lock();
	ACCOUNT_MAP::iterator _cell = m_accountMap.find(_client);

	if( m_accountMap.end() != _cell )
	{
		BASE_OBJECT* _obj = _cell->second;	
		
		_status = _obj->ReadHeader((const unsigned char*)_buff, &_obj->_WebSocketheader);
		if( _status )
		{
			if( _obj->_WebSocketheader.payload_size < 126 )
			{
				NetworkSystem::Instance()->request_Recv(_obj->_pPerHandle, _obj->_pPerIo, OP_READ_DATA, _obj->_WebSocketheader.payload_size - 2 + 4);
			}
			else
			{
				NetworkSystem::Instance()->request_Recv(_obj->_pPerHandle, _obj->_pPerIo, OP_READ_DATA, _obj->_WebSocketheader.extended_size + 4);
			}
		}
	}

	m_lockForAccountMap.unlock();

	return _status;
}

bool ClientObjectSystem::readData(__BRIDGE _client, char* _buff, size_t _len)
{
	bool _status = false;

	m_lockForAccountMap.lock();
	ACCOUNT_MAP::iterator _cell = m_accountMap.find(_client);

	if( m_accountMap.end() != _cell )
	{
		BASE_OBJECT* _obj = _cell->second;	

		if( _obj->parse_buff(_buff, _len) == 1 )
		{
			_status = true;
		}
	}

	m_lockForAccountMap.unlock();

	return _status;
}


void ClientObjectSystem::running()
{
	ACCOUNT_MAP::iterator cell;

	m_lockForAccountMap.lock();

	for ( cell = m_accountMap.begin(); cell != m_accountMap.end(); cell++ )
	{
		BASE_OBJECT* _player = cell->second;

		if( _player != NULL && _player->_bHandshake )
		{

		}
	}

	m_lockForAccountMap.unlock();
}