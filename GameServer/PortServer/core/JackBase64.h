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
    /*���� 
    DataByte 
        [in]��������ݳ���,���ֽ�Ϊ��λ 
    */  
    string Encode(const unsigned char* Data,int DataByte);  
    /*���� 
    DataByte 
        [in]��������ݳ���,���ֽ�Ϊ��λ 
    OutByte 
        [out]��������ݳ���,���ֽ�Ϊ��λ,�벻Ҫͨ������ֵ���� 
        ������ݵĳ��� 
    */  
    string Decode(const char* Data,int DataByte,int& OutByte);  
	};

	//////////////////////////////////////////////////////////////////////////
	extern int GetInterNetURLText(const char* lpcInterNetURL, char*& buff, size_t _bufflen);

	void checkPath(const char* _path);
	void writefile(const char* _path, char* _buff, size_t _buffsize);
};

#endif


