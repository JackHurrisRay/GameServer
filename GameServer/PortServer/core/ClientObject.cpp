
#include <iostream>
#include "ClientObject.h"
#include "WebSocketSystem.h"
#include "./../network/NetworkSystem.h"
#include "./../extern/base64.h"
#include "ProtocalFactory.h"


/************************************************************************/
/* object                                                               */
/************************************************************************/
BASE_OBJECT::BASE_OBJECT()
{

}

BASE_OBJECT::~BASE_OBJECT()
{

}

void BASE_OBJECT::reset()
{
	//////////////////////////////////////////////////////////////////////////
	_UID             = 0xFFFF;

	//////////////////////////////////////////////////////////////////////////
	_bridge_client   = 0;
	_bridge_server   = 0;

	_pPerHandle      = 0;
	_pPerIo          = 0;

	//////////////////////////////////////////////////////////////////////////
	_bHandshake      = false;
	memset(_BUFF1, 0, sizeof(BUFFER_SIZE));
	memset(_BUFF2, 0, sizeof(BUFFER_SIZE));

}

/*
void BASE_OBJECT::release()
{

}
*/

bool BASE_OBJECT::ReadHeader(const unsigned char* cData, WebSocketStreamHeader* header)
{
	if (cData == NULL)return false;

	const unsigned char *buf = cData;

	header->fin = buf[0] & 0x80;
	header->masked = buf[1] & 0x80;
	unsigned char stream_size = buf[1] & 0x7F;

	header->opcode = buf[0] & 0x0F;
	if (header->opcode == WS_FrameType::WS_CONTINUATION_FRAME) {
		//连续帧
		return false;
	}
	else if (header->opcode == WS_FrameType::WS_TEXT_FRAME) {
		//文本帧
	}
	else if (header->opcode == WS_FrameType::WS_BINARY_FRAME) {
		//二进制帧

	}
	else if (header->opcode == WS_FrameType::WS_CLOSING_FRAME) {
		//连接关闭消息
		return false;
	}
	else if (header->opcode == WS_FrameType::WS_PING_FRAME) {
		//  ping
		return false;
	}
	else if (header->opcode == WS_FrameType::WS_PONG_FRAME) {
		// pong
		return false;
	}
	else {
		//非法帧
		return false;
	}

	header->extended_size = 0;
	if (stream_size <= 125) {
		//	small stream
		header->header_size  = 6;
		header->payload_size = stream_size;
		header->mask_offset  = 2;
	}
	else if (stream_size == 126) {
		//	medium stream 
		header->header_size = 8;
		unsigned short s = 0;
		
		s = buf[2] << 8;
		s += buf[3];
	
		header->payload_size  = 126;
		header->extended_size = s;
		header->mask_offset   = 4;
	}
	else if (stream_size == 127) {

		/*
		unsigned long long l = 0;
		memcpy(&l, (const char*)&buf[2], 8);

		header->payload_size = l;
		header->mask_offset = 10;
		*/

		return false;
	}
	else {
		//Couldnt decode stream size 非法大小数据包
		return false;
	}

	if (header->payload_size > MAX_OBJ_LOAD_DATA) {
		return false;
	}

	memset(_BUFF1, 0, sizeof(_BUFF1));
	memcpy(_BUFF1, cData, 4);

	return true;
}

bool BASE_OBJECT::DecodeRawData(const WebSocketStreamHeader& header, BYTE cbSrcData[], WORD wSrcLen, BYTE cbTagData[])
{
	const unsigned char *final_buf = cbSrcData;

	if (wSrcLen < header.header_size + 1) {
		return false;
	}

	unsigned int _data_size = header.payload_size;
	if( header.extended_size > 0 )
	{
		_data_size = header.extended_size;
	}

	char masks[4];
	memcpy(masks, final_buf + header.mask_offset, 4);
	memcpy(cbTagData, final_buf + header.mask_offset + 4, _data_size);

	for (unsigned int i = 0; i < _data_size; ++i){
		cbTagData[i] = (cbTagData[i] ^ masks[i % 4]);
	}

	//如果是文本包，在数据最后加一个结束字符“\0”
	if (header.opcode==WS_FrameType::WS_TEXT_FRAME)
		cbTagData[_data_size] = '\0';

	return true;
}

int BASE_OBJECT::parse_buff(char* _buff, size_t _len)
{
	int _code = -1;
	memcpy(&_BUFF1[4], _buff, _len);

	size_t _parseLen = 0;

	if( _WebSocketheader.payload_size < 126 )
	{
		_parseLen = _WebSocketheader.payload_size + 2;
	}
	else
	{
		_parseLen = _WebSocketheader.extended_size + 4;
	}

	memset(_BUFF2, 0, sizeof(_BUFF2));	
	if( DecodeRawData(_WebSocketheader, (BYTE*)_BUFF1, _parseLen, (BYTE*)_BUFF2) )
	{
		//////////////////////////////////////////////////////////////////////////
		_code = _WebSocketheader.opcode;
	}

	switch (_code)
	{
	case 1:
		{
			//////////////////////////////////////////////////////////////////////////
			//解析协议
			ENUM_PROTOCAL_ERROR _error = CProtocalFactory::Instance()->parse_msg(_BUFF2, this);
			if( _error == PROTOCAL_OK )
			{
				//////////////////////////////////////////////////////////////////////////

			}
			else
			{
				//////////////////////////////////////////////////////////////////////////
				MSG_PROTOCAL_ERROR _errorMsg(_error);

				if( SEND_MSG<MSG_PROTOCAL_ERROR>(_errorMsg, this) )
				{

				}
				else
				{
					_code = -1;
				}
			}

			break;
		}
	}

	return _code;
}

bool BASE_OBJECT::sendMSG(std::string _msg)
{
	bool _check = false;
	memset(_pPerIo->buf, 0, BUFFER_SIZE);

	if( _msg.size() < BUFFER_SIZE - 32 )
	{
		size_t ret = packet_buff(_msg.c_str(), _pPerIo->buf);

		if( ret > 2 )
		{
			//////////////////////////////////////////////////////////////////////////
			NetworkSystem::Instance()->sendMSG(_pPerHandle, _pPerIo, ret, OP_WRITE);
			_check = true;
		}
		else
		{
			//////////////////////////////////////////////////////////////////////////

		}
	}

	return _check;
}

int BASE_OBJECT::packet_buff(std::string _inMessage, char* _OUTBUFF)
{
	//////////////////////////////////////////////////////////////////////////
	std::string _MSG_STR = _inMessage;

	//////////////////////////////////////////////////////////////////////////
	int ret = -1;  

	//////////////////////////////////////////////////////////////////////////
	const unsigned int messageLength = _MSG_STR.size();  
	if (messageLength > BUFFER_SIZE - 10)  
	{  
		// 暂不支持这么长的数据  
		return 0;  
	}  
		
	const int frameHeaderSize = 2 + ((messageLength<126)?0:((messageLength<0xFFFF)?2:8));  
	unsigned char frameHeader[10];  
	memset(frameHeader, 0, sizeof(frameHeader));  

	//////////////////////////////////////////////////////////////////////////
	// fin位为1, 扩展位为0, 操作位为frameType  
	frameHeader[0] = 0x81;  

	// 填充数据长度  
	if( messageLength < 126 )
	{
		frameHeader[1] = (unsigned char)messageLength;
	}
	else if (messageLength < 0xFFFF)  
	{  
		frameHeader[1] = 0x7e;//static_cast<unsigned char>(messageLength);  

		unsigned short _len = (unsigned short)messageLength;		
		
		memcpy(&frameHeader[2], &_len, 2);
		
		unsigned char _temp = frameHeader[2];

		//内存地址取反
		frameHeader[2] = frameHeader[3];
		frameHeader[3] = _temp;
	}  
	else  
	{  
		//超级长数据
		/*
		frameHeader[1] = 0x7f;  
		unsigned short len = (unsigned short)messageLength;  

		frameHeader[2] = 0;
		frameHeader[3] = 0;
		frameHeader[4] = 0;
		frameHeader[5] = 0;
		frameHeader[6] = 0;
		frameHeader[7] = 0;


		memcpy(&frameHeader[8], &len, 2);  
		*/

		return -1;
	}  

	// 填充数据  
	unsigned int frameSize = frameHeaderSize + messageLength;  
	unsigned char *frame = (unsigned char*)_OUTBUFF;  

	memcpy(frame, frameHeader, frameHeaderSize);  
	memcpy(frame + frameHeaderSize, _MSG_STR.c_str(), _MSG_STR.size());  

	//////////////////////////////////////////////////////////////////////////
	ret = frameSize;
	
	return ret;  

}




