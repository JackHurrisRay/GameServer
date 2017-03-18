#include "ProtocalFactory.h"
#include "json/json.h"
#include "JackBase64.h"
#include <exception>


BASE_PROTOCAL_MSG::BASE_PROTOCAL_MSG(unsigned short p):
	_protocal(p),
	_error_code(-1),
	_error_ex(-1)
{

};

BASE_PROTOCAL_MSG::~BASE_PROTOCAL_MSG()
{
	for (size_t i=0; i<_dataLArray.size(); i++)
	{
		_ALLOC_PARAM.releaseData(_dataLArray[i]);
	}
};

void BASE_PROTOCAL_MSG::initializeParam(int count, ...)
{
	va_list args;
	va_start(args, count);
	for( int i=0; i<count; i++)
	{
		std::string arg = va_arg(args, char*);
		_paramArray.push_back(arg);
		_dataLArray.push_back(_ALLOC_PARAM.createData());
	}
	va_end(args);
}

void BASE_PROTOCAL_MSG::package(std::string& output)
{
	Json::Value _root;
	Json::Value _data;

	_root["protocal"] = _protocal;

	//////////////////////////////////////////////////////////////////////////
	_root["error"]    = 0;

	if( _error_code > 0 )
	{
		_root["error"] = _error_code;

		if( _error_ex > 0 )
		{
			_root["error_ex"] = _error_ex;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	for(size_t i=0; i<_paramArray.size(); i++)
	{
		const char* _jsonkey                     = _paramArray[i].c_str();
		const BASE_PROTOCAL_MSG_DATA* _jsondata  = _dataLArray[i];

		switch (_jsondata->_type)
		{
		case BASE_PROTOCAL_MSG_DATA::TSTRING:
			{
				_data[_jsonkey] = _jsondata->_dataString;
				break;
			}
		case BASE_PROTOCAL_MSG_DATA::TNUMBER:
			{
				_data[_jsonkey] = _jsondata->_dataNumber;
				break;
			}
		case BASE_PROTOCAL_MSG_DATA::TSTRINGARRAY:
			{
				for( size_t i=0; i<_jsondata->_dataSArray.size(); i++ )
				{
					_data[_jsonkey].append( _jsondata->_dataSArray[i] );
				}

				break;
			}
		case BASE_PROTOCAL_MSG_DATA::TINTEGERARRAY:
			{
				for( size_t i=0; i<_jsondata->_dataIArray.size(); i++ )
				{
					_data[_jsonkey].append( _jsondata->_dataIArray[i] );
				}

				break;
			}
		default:
			{
				//some error

				break;
			}
		}
	}

	if( _paramArray.size() > 0 )
	{
		_root["data"]     = _data;
	}

	//////////////////////////////////////////////////////////////////////////
	Json::FastWriter writer;
	output = writer.write(_root);
}

bool BASE_PROTOCAL_MSG::checkParam(Json::Value& _data)
{
	bool _check = true;

	for( size_t i=0; i<_paramArray.size(); i++ )
	{
		std::string _key = _paramArray[i];
		Json::Value _element = _data[_key];

		if( _element.type() == 0 )
		{
			_check = false;
			break;
		}
	}

	return _check;
}

//////////////////////////////////////////////////////////////////////////
ALLOC_PROTOCAL_MSG_DATA BASE_PROTOCAL_MSG::_ALLOC_PARAM(5000);

//////////////////////////////////////////////////////////////////////////
CProtocalFactory::CProtocalFactory(void)
{
	m_func = new LP_CALLBACK_NETWORK_C2S_MSG[MAX_CALLBACK_NETWORK_C2S_COUNT];
	memset(m_func, 0, MAX_CALLBACK_NETWORK_C2S_COUNT * sizeof(LP_CALLBACK_NETWORK_C2S_MSG));
}


CProtocalFactory::~CProtocalFactory(void)
{
	delete[] m_func;
}

//////////////////////////////////////////////////////////////////////////
ENUM_PROTOCAL_ERROR CProtocalFactory::parse_msg(const char* _buff, BASE_OBJECT* _client)
{
	ENUM_PROTOCAL_ERROR _check = PROTOCAL_ERROR_UNKNOW;

	//////////////////////////////////////////////////////////////////////////
	//std::string _str = JackBase64::base64_decode( _buff );
	std::string _str = _buff;

	//////////////////////////////////////////////////////////////////////////
	try
	{
		Json::Reader _reader;
		Json::Value  _jsondata;

		if( _reader.parse(_str.c_str(), _jsondata) )
		{
			unsigned long long _protocal = _jsondata["protocal"].asUInt64();
			Json::Value _data = _jsondata["data"];
			if( _protocal > 0 && _protocal < MAX_CALLBACK_NETWORK_C2S_COUNT && _data.type() != 0 )
			{
				//////////////////////////////////////////////////////////////////////////
				//查询是否该协议存在
				LP_CALLBACK_NETWORK_C2S_MSG _func = m_func[_protocal];

				if( _func != NULL )
				{
					if((*_func)(_data, _client))
					{
						_check = PROTOCAL_OK;
					}
					else
					{
						_check = PROTOCAL_ERROR_WRONGDATA;
					}
				}
				else
				{
					_check = PROTOCAL_ERROR_MISSPROTOCAL;
				}
			}
			else
			{
				_check = PROTOCAL_ERROR_MSG_FORMAT;
			}
		}
		else
		{
			_check = PROTOCAL_ERROR_FORMAT;
		}
	}
	catch (...)
	{
		
	}





	return _check;
}

void CProtocalFactory::bind_func(unsigned short _index, LP_CALLBACK_NETWORK_C2S_MSG _func)
{
	if( _index >= 0 && _index < MAX_CALLBACK_NETWORK_C2S_COUNT )
	{
		m_func[_index] = _func;
	}
}

CProtocalFactory* CProtocalFactory::Instance()
{
	static CProtocalFactory _factory;
	return &_factory;
}