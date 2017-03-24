#ifndef _JACKBASE64_H
#define _JACKBASE64_H

#include <string>  

namespace JackBase64
{
	//////////////////////////////////////////////////////////////////////////
	struct GAME_CONFIG
	{
		std::string _SOURCE_PATH;
		std::string _PLAYER_DIR;

		static GAME_CONFIG* Instance()
		{
			static GAME_CONFIG _config;
			return &_config;
		}

	protected:
		GAME_CONFIG();
	};


	//////////////////////////////////////////////////////////////////////////
	std::string base64_encode(unsigned char const* , unsigned int len);  
	std::string base64_decode(std::string const& s);  

	//////////////////////////////////////////////////////////////////////////
	std::string string_To_UTF8(const std::string & str);


	//////////////////////////////////////////////////////////////////////////
	using namespace std;  
  
	class ZBase64  
	{  
	public:  
    /*编码 
    DataByte 
        [in]输入的数据长度,以字节为单位 
    */  
    string Encode(const unsigned char* Data,int DataByte);  
    /*解码 
    DataByte 
        [in]输入的数据长度,以字节为单位 
    OutByte 
        [out]输出的数据长度,以字节为单位,请不要通过返回值计算 
        输出数据的长度 
    */  
    string Decode(const char* Data,int DataByte,int& OutByte);  
	};

	//////////////////////////////////////////////////////////////////////////
	extern int GetInterNetURLText(const char* lpcInterNetURL, char*& buff, size_t _bufflen);

	void checkPath(const char* _path);
	void writefile(const char* _path, char* _buff, size_t _buffsize);
};

#endif


