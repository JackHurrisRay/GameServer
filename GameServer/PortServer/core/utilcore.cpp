#include "utilcore.h"
#include <iostream>
#include <time.h>

struct tm* GET_CURRENT_TIME()
{
	time_t t = time(NULL);
	struct  tm* current_time = localtime(&t);
	return current_time;
}

unsigned int GET_RAND()
{
	struct tm* _time = GET_CURRENT_TIME();
	
	unsigned int _Result = rand();
	_Result += _time->tm_year + _time->tm_mon + _time->tm_mday;
	_Result += _time->tm_hour + _time->tm_min + _time->tm_sec;

	return _Result;
}