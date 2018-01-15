// testTCPServer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "TCPServer.h"


int _tmain(int argc, _TCHAR* argv[])
{
	TCPServer server;
	server.run();

	system("pause");
	return 0;
}
