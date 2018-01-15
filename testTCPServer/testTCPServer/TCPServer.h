#pragma once

class TCPServer
{
public:
	TCPServer();
	virtual ~TCPServer();

public:
	void run();

	/// 接受客户端接入
	void acceptClient();

	/// 关闭客户端
	void closeClient();

	/// 发送数据
	bool SendData(unsigned short nOpcode, const char* pDataBuffer, const unsigned int& nDataSize);

private:
	SOCKET				mServerSocket;					///< 服务器套接字句柄
	sockaddr_in			mServerAddr;					///< 服务器地址

	SOCKET				mAcceptSocket;					///< 接受的客户端套接字句柄
	sockaddr_in			mAcceptAddr;					///< 接收的客户端地址

	char				m_cbSendBuf[NET_PACKET_SIZE];	///< 发送缓存
	ByteBuffer			m_SendBuffer;					///< 发送缓存
};
