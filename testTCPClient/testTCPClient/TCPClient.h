#pragma once

class TCPClient
{
public:
	TCPClient();
	virtual ~TCPClient();

public:
	/// 主循环
	void run();

	/// 处理网络消息
	bool OnNetMessage(const unsigned short& nOpcode, 
		const char* pDataBuffer, unsigned short nDataSize);

	bool OnNetPacket(const NetPacket_Test_POD* pMsg);
	bool OnNetPacket(const NetPacket_Test_Serialize* pMsg);

private:
	SOCKET				mServerSocket;	///< 服务器套接字句柄
	sockaddr_in			mServerAddr;	///< 服务器地址

	char				m_cbRecvBuf[NET_PACKET_SIZE];
	char				m_cbDataBuf[NET_PACKET_SIZE];
	int					m_nRecvSize;

	ByteBuffer			m_RecvBuffer;
};
