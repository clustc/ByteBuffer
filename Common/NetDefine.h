
#pragma once


#include <string>
#include "TypeDefine.h"
#include "ByteBuffer.h"


//////////////////////////////////////////////////////////////////////////


#define NET_PACKET_DATA_SIZE 1024 
#define NET_PACKET_SIZE (sizeof(NetPacketHeader) + NET_PACKET_DATA_SIZE) * 10


//////////////////////////////////////////////////////////////////////////


/// �������ݰ���ͷ
struct NetPacketHeader
{
	unsigned short		wDataSize;	///< ���ݰ���С���������ͷ�ͷ�����ݴ�С
	unsigned short		wOpcode;	///< ������
};

/// �������ݰ�
struct NetPacket
{
	NetPacketHeader		Header;							///< ��ͷ
	unsigned char		Data[NET_PACKET_DATA_SIZE];		///< ����
};



//////////////////////////////////////////////////////////////////////////


/// ���������
enum eNetOpcode
{
	NET_TEST_POD			= 1,	///< ����ԭ������ֱ�ӿ���
	NET_TEST_SERIALIZE		= 2,	///< �������л�
};

/// ����1���������ݰ�����
struct NetPacket_Test_POD
{
	int		nIndex;
	char	arrMessage[512];
};

/// ����2���������ݰ�����
struct NetPacket_Test_Serialize
{
	int			nIndex;
	std::string	strMessage;
};
NET_APPEND(NetPacket_Test_Serialize)
{
	lht << rht.nIndex
		<< rht.strMessage;
	return lht;
};
NET_READ(NetPacket_Test_Serialize)
{
	lht >> rht.nIndex
		>> rht.strMessage;
	return lht;
};

