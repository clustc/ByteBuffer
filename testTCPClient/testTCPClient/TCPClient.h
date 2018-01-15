#pragma once

class TCPClient
{
public:
	TCPClient();
	virtual ~TCPClient();

public:
	/// ��ѭ��
	void run();

	/// ����������Ϣ
	bool OnNetMessage(const unsigned short& nOpcode, 
		const char* pDataBuffer, unsigned short nDataSize);

	bool OnNetPacket(const NetPacket_Test_POD* pMsg);
	bool OnNetPacket(const NetPacket_Test_Serialize* pMsg);

private:
	SOCKET				mServerSocket;	///< �������׽��־��
	sockaddr_in			mServerAddr;	///< ��������ַ

	char				m_cbRecvBuf[NET_PACKET_SIZE];
	char				m_cbDataBuf[NET_PACKET_SIZE];
	int					m_nRecvSize;

	ByteBuffer			m_RecvBuffer;
};
