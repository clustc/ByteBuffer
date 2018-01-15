
#include "stdafx.h"
#include "TCPServer.h"

TCPServer::TCPServer()
: mServerSocket(INVALID_SOCKET)
{
	// 创建套接字
	mServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (mServerSocket == INVALID_SOCKET)
	{
		std::cout << "创建套接字失败!" << std::endl;
		return;
	}

	// 填充服务器的IP和端口号
	mServerAddr.sin_family		= AF_INET;
	mServerAddr.sin_addr.s_addr	= INADDR_ANY;
	mServerAddr.sin_port		= htons((u_short)SERVER_PORT);

	// 绑定IP和端口
	if ( ::bind(mServerSocket, (sockaddr*)&mServerAddr, sizeof(mServerAddr)) == SOCKET_ERROR)
	{
		std::cout << "绑定IP和端口失败!" << std::endl;
		return;
	}

	// 监听客户端请求,最大同时连接数设置为10.
	if ( ::listen(mServerSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "监听端口失败!" << std::endl;
		return;
	}

	std::cout << "启动TCP服务器成功!" << std::endl;
}

TCPServer::~TCPServer()
{
	::closesocket(mServerSocket);
	std::cout << "关闭TCP服务器成功!" << std::endl;
}

void TCPServer::run()
{
	// 接收客户端的连接
	acceptClient();

	int nCount = 0;
	for (;;)
	{
		if (mAcceptSocket == INVALID_SOCKET) 
		{
			std::cout << "客户端主动断开了连接！" << std::endl;
			break;
		}

		// 发送POD数据包
		{
			NetPacket_Test_POD msg;
			msg.nIndex = nCount;
			strncpy(msg.arrMessage, "[1]你好[2]你好[3]你好", sizeof(msg.arrMessage) );
			bool bRet = SendData(NET_TEST_POD, (const char*)&msg, sizeof(msg));
			if (bRet)
			{
				std::cout << "发送数据成功！" << std::endl;
			}
			else
			{
				std::cout << "发送数据失败！" << std::endl;
				break;
			}
		}

		// 发送序列化数据包
		{
			NetPacket_Test_Serialize msg;
			msg.nIndex = nCount;
			msg.strMessage = "[1]早晨[2]早晨[3]早晨";

			// 清除缓存
			m_SendBuffer.clear();
			// 序列化
			m_SendBuffer << msg;

			bool bRet = SendData(NET_TEST_SERIALIZE, 
				(const char*)m_SendBuffer.contents(),
				(const unsigned int) m_SendBuffer.size());
			if (bRet)
			{
				std::cout << "发送数据成功！" << std::endl;
			}
			else
			{
				std::cout << "发送数据失败！" << std::endl;
				break;
			}
		}

		++nCount;
	}
}

void TCPServer::closeClient()
{
	// 判断套接字是否有效
	if (mAcceptSocket == INVALID_SOCKET) return;

	// 关闭客户端套接字
	::closesocket(mAcceptSocket);
	std::cout << "客户端套接字已关闭!" << std::endl;
}

void TCPServer::acceptClient()
{
	// 以阻塞方式,等待接收客户端连接
	int nAcceptAddrLen = sizeof(mAcceptAddr);
	mAcceptSocket = ::accept(mServerSocket, (struct sockaddr*)&mAcceptAddr, &nAcceptAddrLen);
	std::cout << "接受客户端IP:" << inet_ntoa(mAcceptAddr.sin_addr) << std::endl;
}

bool TCPServer::SendData( unsigned short nOpcode, const char* pDataBuffer, const unsigned int& nDataSize )
{
	NetPacketHeader* pHead = (NetPacketHeader*) m_cbSendBuf;
	pHead->wOpcode = nOpcode;

	// 数据封包
	if ( (nDataSize > 0) && (pDataBuffer != 0) )
	{
		CopyMemory(pHead+1, pDataBuffer, nDataSize);
	}

	// 发送消息
	const unsigned short nSendSize = nDataSize + sizeof(NetPacketHeader);
	pHead->wDataSize = nSendSize;
	int ret = ::send(mAcceptSocket, m_cbSendBuf, nSendSize, 0);
	return (ret > 0) ? true : false;
}
