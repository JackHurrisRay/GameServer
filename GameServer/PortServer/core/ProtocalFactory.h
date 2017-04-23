#ifndef _PROTOCALFACTORY_H
#define _PROTOCALFACTORY_H

enum ENUM_PROTOCAL_ERROR
{
	PROTOCAL_OK                  = 0,
	PROTOCAL_ERROR_UNKNOW        = 1,
	PROTOCAL_ERROR_FORMAT        = 101,       //协议格式错误
	PROTOCAL_ERROR_MSG_FORMAT    = 102,       //协议内容格式错误
	PROTOCAL_ERROR_MISSPROTOCAL  = 103,       //协议不存在
	PROTOCAL_ERROR_WRONGDATA     = 104,       //错误数据

	//////////////////////////////////////////////////////////////////////////
	PROTOCAL_ERROR_LOGINFAILED   = 201,       //登录失败
	PROTOCAL_ERROR_NONELOGIN     = 202,       //没有登录，协议无法通讯

	//////////////////////////////////////////////////////////////////////////
	PROTOCAL_ERROR_ROOM          = 301,       //房间相关错误
	PROTOCAL_ERROR_GAME          = 401,       //游戏过程中状态错误

	PROTOCAL_ERROR_PLAYER        = 501,       //玩家相关错误

};



#include <stdarg.h>
#include <string>
#include <vector>
#include "MemAllocPool.h"

#define  EGP_ERROR 404

//////////////////////////////////////////////////////////////////////////
namespace Json{
	class Value;
};

struct BASE_OBJECT;

struct BASE_PROTOCAL_MSG_DATA;
typedef std::vector<std::string> STRING_ARRAY;
typedef std::vector<int>         INTEGER_ARRAY;
typedef std::vector<BASE_PROTOCAL_MSG_DATA*> BASE_PROTOCAL_MSG_DATA_ARRAY;


//////////////////////////////////////////////////////////////////////////
struct BASE_PROTOCAL_MSG_DATA
{
	enum TYPE
	{
		TNONE = 0,
		TSTRING,
		TNUMBER,
		TSTRINGARRAY,
		TINTEGERARRAY,
	};

	TYPE _type;
    long long _dataNumber;
	std::string        _dataString;
	STRING_ARRAY       _dataSArray;
	INTEGER_ARRAY      _dataIArray;

	inline void setString(const char* cstr)
	{
		_type       = TSTRING;
		_dataString = cstr;
	}

	inline void setNumber(long long number)
	{
		_type       = TNUMBER;
		_dataNumber = number;
	}

	inline void setSArray(STRING_ARRAY _strArray)
	{
		_type       = TSTRINGARRAY;
		for( size_t i=0; i<_strArray.size(); i++ )
		{
			_dataSArray.push_back(_strArray[i]);
		}
	}

	inline void setIArray(INTEGER_ARRAY _intArray)
	{
		_type       = TINTEGERARRAY;
		for( size_t i=0; i<_intArray.size(); i++ )
		{
			_dataIArray.push_back(_intArray[i]);
		}
	}
};
typedef MemAllocPool<BASE_PROTOCAL_MSG_DATA> ALLOC_PROTOCAL_MSG_DATA;

struct BASE_PROTOCAL_MSG
{
	static ALLOC_PROTOCAL_MSG_DATA _ALLOC_PARAM;

	//////////////////////////////////////////////////////////////////////////
	const unsigned short           _protocal;
	int                            _error_code;
	int                            _error_ex;

	//////////////////////////////////////////////////////////////////////////
	STRING_ARRAY                   _paramArray;
	BASE_PROTOCAL_MSG_DATA_ARRAY   _dataLArray;

	BASE_PROTOCAL_MSG(unsigned short p);
	virtual ~BASE_PROTOCAL_MSG();
	
	void initializeParam(int count, ...);	

	//s2c
	void package(std::string& output);

	//c2s check
	bool checkParam(Json::Value& _data);

};

template<class T>
bool CHECK_PARAM(Json::Value& data)
{
	T t;
	return ( t.checkParam(data) );
};

struct MSG_PROTOCOL_SIMPLER:
	public BASE_PROTOCAL_MSG
{
	MSG_PROTOCOL_SIMPLER():BASE_PROTOCAL_MSG(1)
	{
		initializeParam(4, "player_id", "room_id", "player_name", "player_score");
	};
};

struct MSG_PROTOCAL_ERROR:
	public BASE_PROTOCAL_MSG
{
	MSG_PROTOCAL_ERROR(int error_code):BASE_PROTOCAL_MSG(EGP_ERROR)
	{
		_error_code = error_code;
	}
};

//////////////////////////////////////////////////////////////////////////
typedef bool CALLBACK_NETWORK_C2S_MSG(Json::Value&, BASE_OBJECT*);
typedef CALLBACK_NETWORK_C2S_MSG* LP_CALLBACK_NETWORK_C2S_MSG;
#define  MAX_CALLBACK_NETWORK_C2S_COUNT 4096


class CProtocalFactory
{
protected:
	CProtocalFactory(void);
	~CProtocalFactory(void);

	LP_CALLBACK_NETWORK_C2S_MSG* m_func;

public:

	static CProtocalFactory* Instance();

	ENUM_PROTOCAL_ERROR parse_msg(const char* _buff, BASE_OBJECT* _client);

	void bind_func(unsigned short _index, LP_CALLBACK_NETWORK_C2S_MSG _func);
};

#endif
