#ifndef _JACKBASE64_H
#define _JACKBASE64_H

#include <string>  

namespace JackBase64
{
	//////////////////////////////////////////////////////////////////////////
	std::string base64_encode(unsigned char const* , unsigned int len);  
	std::string base64_decode(std::string const& s);  

	//////////////////////////////////////////////////////////////////////////
	std::string string_To_UTF8(const std::string & str);
};

#endif


