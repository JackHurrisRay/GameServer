#ifndef _CLIENTOBJECT_H
#define _CLIENTOBJECT_H
#include "WebSocketCommon.h"
#include "JackBase64.h"

#include <map>
#include "MemAllocPool.h"

//////////////////////////////////////////////////////////////////////////
#define MAX_OBJ_LOAD_DATA 2048

//Websocket���ݰ�����ͷ��Ϣ  
struct WebSocketStreamHeader {  
	unsigned char  fin;                                 //֡���  
	unsigned char  res[3];                     
	unsigned char  opcode;                              //������  
	unsigned char  masked;                              //����  
	unsigned char  payload_size;                        //���ݴ�С  
	unsigned char  header_size;                         //���ݰ�ͷ��С  
	unsigned char  mask_offset;                         //����ƫ��  
	unsigned short extended_size;                       //�������ݴ�С
};  

//
enum WS_FrameType  
{  
	WS_CONTINUATION_FRAME = 0x00,
	WS_EMPTY_FRAME        = 0xF0,  
	WS_ERROR_FRAME        = 0xF1,  
	WS_TEXT_FRAME         = 0x01,  
	WS_BINARY_FRAME       = 0x02,  
	WS_PING_FRAME         = 0x09,  
	WS_PONG_FRAME         = 0x0A,  
	WS_OPENING_FRAME      = 0xF3,  
	WS_CLOSING_FRAME      = 0x08  
}; 

//////////////////////////////////////////////////////////////////////////
struct BASE_MSG
{
	char _buff[BUFFER_SIZE];
};

typedef MemAllocPool<BASE_MSG> BASE_MSG_ALLOC;
typedef std::list<BASE_MSG*>   BASE_MSG_LIST;

struct PER_HANDLE_DATA;
struct PER_IO_DATA;

struct BASE_OBJECT
{
	/************************************************************************/
	/* DATA                                                                 */
	/************************************************************************/
	unsigned short         _UID;

	/************************************************************************/
	/* Network                                                              */
	/************************************************************************/
	//////////////////////////////////////////////////////////////////////////
	char                   _BUFF1[BUFFER_SIZE];
	char                   _BUFF2[BUFFER_SIZE];

	//////////////////////////////////////////////////////////////////////////
	__BRIDGE               _bridge_client;   //connnect to client
	__BRIDGE               _bridge_server;   //connect to gameserver

	PER_HANDLE_DATA*       _pPerHandle;
	PER_IO_DATA*           _pPerIo;

	//////////////////////////////////////////////////////////////////////////
	WebSocketStreamHeader  _WebSocketheader;

	bool                   _bHandshake;      //���ֳɹ�

	/************************************************************************/
	/* Method                                                               */
	/************************************************************************/
	//////////////////////////////////////////////////////////////////////////
	BASE_OBJECT();
	~BASE_OBJECT();
	void reset();
	void release();

	//////////////////////////////////////////////////////////////////////////
	int  parse_buff(char* _buff_from, size_t _len);
	int  packet_buff(std::string inMessage, char* _OUT_BUFF);

	//////////////////////////////////////////////////////////////////////////
	bool ReadHeader(const unsigned char* cData, WebSocketStreamHeader* header);
	bool DecodeRawData(const WebSocketStreamHeader& header, BYTE cbSrcData[], WORD wSrcLen, BYTE cbTagData[]);

	bool sendMSG(std::string _msg);
};

typedef MemAllocPool<BASE_OBJECT> BASE_OBJECT_ALLOC;
typedef std::map<__BRIDGE, BASE_OBJECT*> ACCOUNT_MAP;

template<class Message>
bool SEND_MSG(Message msg, BASE_OBJECT* client)
{
	//////////////////////////////////////////////////////////////////////////
	Message& _Msg = msg;
	std::string _output;
	_Msg.package(_output);

	//////////////////////////////////////////////////////////////////////////
	std::string _OUT = _output;//JackBase64::base64_encode((const unsigned char*)_output.c_str(), _output.size());

	//////////////////////////////////////////////////////////////////////////
	return client->sendMSG(_OUT);
}

#endif // !_CLIENTOBJECT_H



