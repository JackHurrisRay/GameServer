#include "JackBase64.h"
#include <iostream>  
#include <fstream>
#include <wchar.h>
#include <windows.h>
#include <urlmon.h>
#include <io.h>
#include <wininet.h>
#pragma comment(lib, "wininet.lib")

#include "json/json.h"

namespace JackBase64
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	GAME_CONFIG::GAME_CONFIG()
	{
		const char* _configFilePath = "data/config.ini";
		char  _BUFF[MAX_PATH];
		DWORD _hr;

		_hr = GetPrivateProfileStringA("SERVER", "SOURCE_PATH", "", _BUFF, sizeof(_BUFF), _configFilePath);
		_SOURCE_PATH = _BUFF;

		_PLAYER_PATH = "data\\player\\";

		//////////////////////////////////////////////////////////////////////////
		_GAME_MAX_AROUND[0] = GetPrivateProfileIntA("GAME","GAME_MAX_AROUND_1",0,_configFilePath);
		_GAME_MAX_AROUND[1] = GetPrivateProfileIntA("GAME","GAME_MAX_AROUND_2",0,_configFilePath);
		_GAME_MAX_AROUND[2] = GetPrivateProfileIntA("GAME","GAME_MAX_AROUND_3",0,_configFilePath);

		//////////////////////////////////////////////////////////////////////////
		_GAME_MAX_AROUND_GOLD[0] = GetPrivateProfileIntA("GAME","GMA_GOLD_1",0,_configFilePath);
		_GAME_MAX_AROUND_GOLD[1] = GetPrivateProfileIntA("GAME","GMA_GOLD_2",0,_configFilePath);
		_GAME_MAX_AROUND_GOLD[2] = GetPrivateProfileIntA("GAME","GMA_GOLD_3",0,_configFilePath);

		//////////////////////////////////////////////////////////////////////////
		_GAME_VIP_GOLD[0] = GetPrivateProfileIntA("GAME","GAME_DAY_VIP",0,_configFilePath);
		_GAME_VIP_GOLD[1] = GetPrivateProfileIntA("GAME","GAME_WEAK_VIP",0,_configFilePath);
		_GAME_VIP_GOLD[2] = GetPrivateProfileIntA("GAME","GAME_MONTH_VIP",0,_configFilePath);

		const char* _VIP_GOLD_NAME[] = {"DAY","WEAK","MONTH"};

		Json::Value _root;
		Json::Value _data;
		Json::Value _GAME_MAX_AROUND;
		Json::Value _GAME_VIP_GOLD;

		for( int i = 0; i<3; i++ )
		{
			Json::Value _max_around_data;
			_max_around_data["COUNT"] = this->_GAME_MAX_AROUND[i];
			_max_around_data["GOLD"]  = this->_GAME_MAX_AROUND_GOLD[i];

			_data["MAX_AROUND"].append(_max_around_data);

			Json::Value _vip_gold_data;
			_vip_gold_data["NAME"] = _VIP_GOLD_NAME[i];
			_vip_gold_data["GOLD"] = this->_GAME_VIP_GOLD[i];

			_data["VIP_TYPE"].append(_vip_gold_data);
		}

		_root["DATA"] = _data;

		Json::FastWriter writer;
		std::string output = writer.write(_root);

		_JSON_DATA_FOR_GAME = output;

	}


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	//////////////////////////////////////////////////////////////////////////
	static const std::string base64_chars =   
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"  
		"abcdefghijklmnopqrstuvwxyz"  
		"0123456789+/";  

	//////////////////////////////////////////////////////////////////////////
	static inline bool is_base64(unsigned char c) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	};

	std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
		std::string ret;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];

		while (in_len--) {
			char_array_3[i++] = *(bytes_to_encode++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for(i = 0; (i <4) ; i++)
					ret += base64_chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i)
		{
			for(j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (j = 0; (j < i + 1); j++)
				ret += base64_chars[char_array_4[j]];

			while((i++ < 3))
				ret += '=';

		}

		return ret;

	}

	std::string base64_decode(std::string const& encoded_string) {
		int in_len = encoded_string.size();
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		std::string ret;

		while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
			char_array_4[i++] = encoded_string[in_]; in_++;
			if (i ==4) {
				for (i = 0; i <4; i++)
					char_array_4[i] = base64_chars.find(char_array_4[i]);

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
					ret += char_array_3[i];
				i = 0;
			}
		}

		if (i) {
			for (j = i; j <4; j++)
				char_array_4[j] = 0;

			for (j = 0; j <4; j++)
				char_array_4[j] = base64_chars.find(char_array_4[j]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
		}

		return ret;
	}

	std::string string_To_UTF8(const std::string & str)
	{
		int nwLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

		wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
		memset(pwBuf, 0, nwLen * 2 + 2);

		MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

		int nLen = WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

		char * pBuf = new char[nLen + 1];
		memset(pBuf, 0, nLen + 1);

		WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

		std::string retStr(pBuf);

		delete []pwBuf;
		delete []pBuf;

		pwBuf = NULL;
		pBuf = NULL;

		return retStr;
	}
	//////////////////////////////////////////////////////////////////////////

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	string ZBase64::Encode(const unsigned char* Data,int DataByte)  
	{  
		//编码表  
		const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";  
		//返回值  
		string strEncode;  
		unsigned char Tmp[4]={0};  
		int LineLength=0;  
		for(int i=0;i<(int)(DataByte / 3);i++)  
		{  
			Tmp[1] = *Data++;  
			Tmp[2] = *Data++;  
			Tmp[3] = *Data++;  
			strEncode+= EncodeTable[Tmp[1] >> 2];  
			strEncode+= EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];  
			strEncode+= EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];  
			strEncode+= EncodeTable[Tmp[3] & 0x3F];  
			if(LineLength+=4,LineLength==76) {strEncode+="\r\n";LineLength=0;}  
		}  
		//对剩余数据进行编码  
		int Mod=DataByte % 3;  
		if(Mod==1)  
		{  
			Tmp[1] = *Data++;  
			strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];  
			strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4)];  
			strEncode+= "==";  
		}  
		else if(Mod==2)  
		{  
			Tmp[1] = *Data++;  
			Tmp[2] = *Data++;  
			strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];  
			strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];  
			strEncode+= EncodeTable[((Tmp[2] & 0x0F) << 2)];  
			strEncode+= "=";  
		}  

		return strEncode;  
	}  

	string ZBase64::Decode(const char* Data,int DataByte,int& OutByte)  
	{  
		//解码表  
		const char DecodeTable[] =  
		{  
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
			62, // '+'  
			0, 0, 0,  
			63, // '/'  
			52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'  
			0, 0, 0, 0, 0, 0, 0,  
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,  
			13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'  
			0, 0, 0, 0, 0, 0,  
			26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,  
			39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'  
		};  
		//返回值  
		string strDecode;  
		int nValue;  
		int i= 0;  
		while (i < DataByte)  
		{  
			if (*Data != '\r' && *Data!='\n')  
			{  
				nValue = DecodeTable[*Data++] << 18;  
				nValue += DecodeTable[*Data++] << 12;  
				strDecode+=(nValue & 0x00FF0000) >> 16;  
				OutByte++;  
				if (*Data != '=')  
				{  
					nValue += DecodeTable[*Data++] << 6;  
					strDecode+=(nValue & 0x0000FF00) >> 8;  
					OutByte++;  
					if (*Data != '=')  
					{  
						nValue += DecodeTable[*Data++];  
						strDecode+=nValue & 0x000000FF;  
						OutByte++;  
					}  
				}  
				i += 4;  
			}  
			else// 回车换行,跳过  
			{  
				Data++;  
				i++;  
			}  
		}  
		return strDecode;  
	}  

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	int GetInterNetURLText(const char* lpcInterNetURL, char*& buff, size_t _bufflen)  
	{   
		int _readlen = 0;
		HINTERNET hSession;     
		hSession = InternetOpenA("WinInet", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);     
		__try  
		{         
			if(hSession != NULL)          
			{            
				HINTERNET hRequest;              
				hRequest = InternetOpenUrlA(hSession,lpcInterNetURL, NULL,0, INTERNET_FLAG_RELOAD, 0);          
				__try           
				{                  
					if(hRequest != NULL)          
					{             
						DWORD dwBytesRead;                    
						char*& szBuffer = buff;  

						if(InternetReadFile(hRequest, szBuffer, _bufflen, &dwBytesRead))             
						{                   
							_readlen = dwBytesRead;               
						}                 
					}             
				}__finally       
				{                
					InternetCloseHandle(hRequest);    
				}         
			}     
		}__finally      
		{         
			InternetCloseHandle(hSession);    
		}     

		return _readlen;
	}  

	void checkPath(const char* _path)
	{
		int status = -1;
		if( access(_path, 0) != 0 )
		{
			CreateDirectoryA(_path, NULL);
		}

		return;
	}

	void writefile(const char* _path, char* _buff, size_t _buffsize)
	{
		FILE* _file = fopen(_path, "wb");

		if( _file )
		{
			fwrite(_buff, sizeof(char), _buffsize, _file);
			fclose(_file);
		}
	}

	void writeTextToFile(const char* _path, std::string _text)
	{
		FILE* _file = fopen(_path, "w");
		if( _file )
		{
			fwrite(_text.c_str(), sizeof(char), _text.size(), _file);
			fclose(_file);
		}
	}

	bool readTextFromFile(const char* _path, std::string& _text)
	{
		bool _check = false;
		FILE* _file = fopen(_path, "r");
		if( _file )
		{
			char _buff[4096];
			memset(_buff, 0, sizeof(_buff));
			if( fread(_buff, sizeof(char), sizeof(_buff), _file) > 0 )
			{
				_text  = _buff;
				_check = true;
			}

			fclose(_file);
		}

		return _check;
	}

};

