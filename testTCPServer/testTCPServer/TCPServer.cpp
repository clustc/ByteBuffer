
#include "stdafx.h"
#include "TCPServer.h"

TCPServer::TCPServer()
: mServerSocket(INVALID_SOCKET)
{
	// �����׽���
	mServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (mServerSocket == INVALID_SOCKET)
	{
		std::cout << "�����׽���ʧ��!" << std::endl;
		return;
	}

	// ����������IP�Ͷ˿ں�
	mServerAddr.sin_family		= AF_INET;
	mServerAddr.sin_addr.s_addr	= INADDR_ANY;
	mServerAddr.sin_port		= htons((u_short)SERVER_PORT);

	// ��IP�Ͷ˿�
	if ( ::bind(mServerSocket, (sockaddr*)&mServerAddr, sizeof(mServerAddr)) == SOCKET_ERROR)
	{
		std::cout << "��IP�Ͷ˿�ʧ��!" << std::endl;
		return;
	}

	// �����ͻ�������,���ͬʱ����������Ϊ10.
	if ( ::listen(mServerSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "�����˿�ʧ��!" << std::endl;
		return;
	}

	std::cout << "����TCP�������ɹ�!" << std::endl;
}

TCPServer::~TCPServer()
{
	::closesocket(mServerSocket);
	std::cout << "�ر�TCP�������ɹ�!" << std::endl;
}

void TCPServer::run()
{
	// ���տͻ��˵�����
	acceptClient();

	int nCount = 0;
	for (;;)
	{
		if (mAcceptSocket == INVALID_SOCKET) 
		{
			std::cout << "�ͻ��������Ͽ������ӣ�" << std::endl;
			break;
		}

		// ����POD���ݰ�
		{
			NetPacket_Test_POD msg;
			msg.nIndex = nCount;
			strncpy(msg.arrMessage, "[1]���[2]���[3]���", sizeof(msg.arrMessage) );
			bool bRet = SendData(NET_TEST_POD, (const char*)&msg, sizeof(msg));
			if (bRet)
			{
				std::cout << "�������ݳɹ���" << std::endl;
			}
			else
			{
				std::cout << "��������ʧ�ܣ�" << std::endl;
				break;
			}
		}

		// �������л����ݰ�
		{
			NetPacket_Test_Serialize msg;
			msg.nIndex = nCount;
			msg.strMessage = "[1]�糿[2]�糿[3]�糿";

			// �������
			m_SendBuffer.clear();
			// ���л�
			m_SendBuffer << msg;

			bool bRet = SendData(NET_TEST_SERIALIZE, 
				(const char*)m_SendBuffer.contents(),
				(const unsigned int) m_SendBuffer.size());
			if (bRet)
			{
				std::cout << "�������ݳɹ���" << std::endl;
			}
			else
			{
				std::cout << "��������ʧ�ܣ�" << std::endl;
				break;
			}
		}

		++nCount;
	}
}

void TCPServer::closeClient()
{
	// �ж��׽����Ƿ���Ч
	if (mAcceptSocket == INVALID_SOCKET) return;

	// �رտͻ����׽���
	::closesocket(mAcceptSocket);
	std::cout << "�ͻ����׽����ѹر�!" << std::endl;
}

void TCPServer::acceptClient()
{
	// ��������ʽ,�ȴ����տͻ�������
	int nAcceptAddrLen = sizeof(mAcceptAddr);
	mAcceptSocket = ::accept(mServerSocket, (struct sockaddr*)&mAcceptAddr, &nAcceptAddrLen);
	std::cout << "���ܿͻ���IP:" << inet_ntoa(mAcceptAddr.sin_addr) << std::endl;
}

bool TCPServer::SendData( unsigned short nOpcode, const char* pDataBuffer, const unsigned int& nDataSize )
{
	NetPacketHeader* pHead = (NetPacketHeader*) m_cbSendBuf;
	pHead->wOpcode = nOpcode;

	// ���ݷ��
	if ( (nDataSize > 0) && (pDataBuffer != 0) )
	{
		CopyMemory(pHead+1, pDataBuffer, nDataSize);
	}

	// ������Ϣ
	const unsigned short nSendSize = nDataSize + sizeof(NetPacketHeader);
	pHead->wDataSize = nSendSize;
	int ret = ::send(mAcceptSocket, m_cbSendBuf, nSendSize, 0);
	return (ret > 0) ? true : false;
}
