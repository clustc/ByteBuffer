#pragma once

class TCPServer
{
public:
	TCPServer();
	virtual ~TCPServer();

public:
	void run();

	/// ���ܿͻ��˽���
	void acceptClient();

	/// �رտͻ���
	void closeClient();

	/// ��������
	bool SendData(unsigned short nOpcode, const char* pDataBuffer, const unsigned int& nDataSize);

private:
	SOCKET				mServerSocket;					///< �������׽��־��
	sockaddr_in			mServerAddr;					///< ��������ַ

	SOCKET				mAcceptSocket;					///< ���ܵĿͻ����׽��־��
	sockaddr_in			mAcceptAddr;					///< ���յĿͻ��˵�ַ

	char				m_cbSendBuf[NET_PACKET_SIZE];	///< ���ͻ���
	ByteBuffer			m_SendBuffer;					///< ���ͻ���
};
