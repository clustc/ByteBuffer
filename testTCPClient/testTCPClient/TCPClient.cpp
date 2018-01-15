
#include "stdafx.h"
#include "TCPClient.h"


TCPClient::TCPClient()
{
	memset( m_cbRecvBuf, 0, sizeof(m_cbRecvBuf) );
	m_nRecvSize = 0;

	// 创建套接字
	mServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (mServerSocket == INVALID_SOCKET)
	{
		std::cout << "创建套接字失败!" << std::endl;
		return;
	}

	// 填充服务器的IP和端口号
	mServerAddr.sin_family		= AF_INET;
	mServerAddr.sin_addr.s_addr	= inet_addr(SERVER_IP);
	mServerAddr.sin_port		= htons((u_short)SERVER_PORT);

	// 连接到服务器
	if ( ::connect(mServerSocket, (struct sockaddr*)&mServerAddr, sizeof(mServerAddr)))
	{
		::closesocket(mServerSocket);
		std::cout << "连接服务器失败!" << std::endl;
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
		// 接收数据
		int nRecvSize = ::recv(mServerSocket,
			m_cbRecvBuf+m_nRecvSize, 
			sizeof(m_cbRecvBuf)-m_nRecvSize, 0);
		if (nRecvSize <= 0)
		{
			std::cout << "服务器主动断开连接!" << std::endl;
			break;
		}

		// 保存已经接收数据的大小
		m_nRecvSize += nRecvSize;

		// 接收到的数据够不够一个包头的长度
		while (m_nRecvSize >= sizeof(NetPacketHeader))
		{
			// 收够10个包，主动与服务器断开
			if (nCount >= 10)
			{
				::closesocket(mServerSocket);
				break;
			}

			// 读取包头
			NetPacketHeader* pHead = (NetPacketHeader*) (m_cbRecvBuf);
			const unsigned short nPacketSize = pHead->wDataSize;

			// 判断是否已接收到足够一个完整包的数据
			if (m_nRecvSize < nPacketSize)
			{
				// 还不够拼凑出一个完整包
				break;
			}

			// 拷贝到数据缓存
			CopyMemory(m_cbDataBuf, m_cbRecvBuf, nPacketSize);

			// 从接收缓存移除
			MoveMemory(m_cbRecvBuf, m_cbRecvBuf+nPacketSize, m_nRecvSize);
			m_nRecvSize -= nPacketSize;

			// 解密数据，以下省略一万字
			// ...

			// 分派数据包，让应用层进行逻辑处理
			pHead = (NetPacketHeader*) (m_cbDataBuf);
			const unsigned short nDataSize = nPacketSize - (unsigned short)sizeof(NetPacketHeader);
			OnNetMessage(pHead->wOpcode, m_cbDataBuf+sizeof(NetPacketHeader), nDataSize);

			++nCount;
		}
	}

	std::cout << "已经和服务器断开连接!" << std::endl;
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
			// 清除缓存中数据
			m_RecvBuffer.clear();
			// 将数据加入到缓存中去
			m_RecvBuffer.append((uint8*)pDataBuffer, nDataSize);

			// 反序列化数据
			NetPacket_Test_Serialize msg;
			m_RecvBuffer >> msg;

			return OnNetPacket(&msg);
		}
		break;

	default:
		{
			std::cout << "收取到未知网络数据包:" << nOpcode << std::endl;
			return false;
		}
		break;
	}
}

bool TCPClient::OnNetPacket( const NetPacket_Test_POD* pMsg )
{
	std::cout << "处理POD数据：" << std::endl;
	std::cout << "索引：" << pMsg->nIndex << "  字符串：" << pMsg->arrMessage << std::endl;
	return true;
}

bool TCPClient::OnNetPacket( const NetPacket_Test_Serialize* pMsg )
{
	std::cout << "处理序列化数据：" << std::endl;
	std::cout << "索引：" << pMsg->nIndex << "  字符串：" << pMsg->strMessage << std::endl;
	return true;
}
