
#include "stdafx.h"
#include "TCPClient.h"


TCPClient::TCPClient()
{
	memset( m_cbRecvBuf, 0, sizeof(m_cbRecvBuf) );
	m_nRecvSize = 0;

	// �����׽���
	mServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (mServerSocket == INVALID_SOCKET)
	{
		std::cout << "�����׽���ʧ��!" << std::endl;
		return;
	}

	// ����������IP�Ͷ˿ں�
	mServerAddr.sin_family		= AF_INET;
	mServerAddr.sin_addr.s_addr	= inet_addr(SERVER_IP);
	mServerAddr.sin_port		= htons((u_short)SERVER_PORT);

	// ���ӵ�������
	if ( ::connect(mServerSocket, (struct sockaddr*)&mServerAddr, sizeof(mServerAddr)))
	{
		::closesocket(mServerSocket);
		std::cout << "���ӷ�����ʧ��!" << std::endl;
		return;	
	}
}

TCPClient::~TCPClient()
{
	::closesocket(mServerSocket);
}

void TCPClient::run()
{
	int nCount = 0;
	for (;;)
	{
		// ��������
		int nRecvSize = ::recv(mServerSocket,
			m_cbRecvBuf+m_nRecvSize, 
			sizeof(m_cbRecvBuf)-m_nRecvSize, 0);
		if (nRecvSize <= 0)
		{
			std::cout << "�����������Ͽ�����!" << std::endl;
			break;
		}

		// �����Ѿ��������ݵĴ�С
		m_nRecvSize += nRecvSize;

		// ���յ������ݹ�����һ����ͷ�ĳ���
		while (m_nRecvSize >= sizeof(NetPacketHeader))
		{
			// �չ�10������������������Ͽ�
			if (nCount >= 10)
			{
				::closesocket(mServerSocket);
				break;
			}

			// ��ȡ��ͷ
			NetPacketHeader* pHead = (NetPacketHeader*) (m_cbRecvBuf);
			const unsigned short nPacketSize = pHead->wDataSize;

			// �ж��Ƿ��ѽ��յ��㹻һ��������������
			if (m_nRecvSize < nPacketSize)
			{
				// ������ƴ�ճ�һ��������
				break;
			}

			// ���������ݻ���
			CopyMemory(m_cbDataBuf, m_cbRecvBuf, nPacketSize);

			// �ӽ��ջ����Ƴ�
			MoveMemory(m_cbRecvBuf, m_cbRecvBuf+nPacketSize, m_nRecvSize);
			m_nRecvSize -= nPacketSize;

			// �������ݣ�����ʡ��һ����
			// ...

			// �������ݰ�����Ӧ�ò�����߼�����
			pHead = (NetPacketHeader*) (m_cbDataBuf);
			const unsigned short nDataSize = nPacketSize - (unsigned short)sizeof(NetPacketHeader);
			OnNetMessage(pHead->wOpcode, m_cbDataBuf+sizeof(NetPacketHeader), nDataSize);

			++nCount;
		}
	}

	std::cout << "�Ѿ��ͷ������Ͽ�����!" << std::endl;
}

bool TCPClient::OnNetMessage( const unsigned short& nOpcode, 
							 const char* pDataBuffer, unsigned short nDataSize )
{
	switch (nOpcode)
	{
	case NET_TEST_POD:
		{
			NetPacket_Test_POD* pMsg = (NetPacket_Test_POD*) pDataBuffer;
			return OnNetPacket(pMsg);
		}
		break;

	case NET_TEST_SERIALIZE:
		{
			// �������������
			m_RecvBuffer.clear();
			// �����ݼ��뵽������ȥ
			m_RecvBuffer.append((uint8*)pDataBuffer, nDataSize);

			// �����л�����
			NetPacket_Test_Serialize msg;
			m_RecvBuffer >> msg;

			return OnNetPacket(&msg);
		}
		break;

	default:
		{
			std::cout << "��ȡ��δ֪�������ݰ�:" << nOpcode << std::endl;
			return false;
		}
		break;
	}
}

bool TCPClient::OnNetPacket( const NetPacket_Test_POD* pMsg )
{
	std::cout << "����POD���ݣ�" << std::endl;
	std::cout << "������" << pMsg->nIndex << "  �ַ�����" << pMsg->arrMessage << std::endl;
	return true;
}

bool TCPClient::OnNetPacket( const NetPacket_Test_Serialize* pMsg )
{
	std::cout << "�������л����ݣ�" << std::endl;
	std::cout << "������" << pMsg->nIndex << "  �ַ�����" << pMsg->strMessage << std::endl;
	return true;
}
