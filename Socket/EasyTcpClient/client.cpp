#include <iostream>
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 0
#include <Windows.h>
#include <WinSock2.h>


int main()
{
	// 启动 Windows socket 2.x 环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	// -----------------------------
	// 1. 建立一个socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock)
	{
		std::cout << "ERROR，创建套接字失败..." << std::endl;
	}
	else
	{
		std::cout << "SUCCESS，创建套接字成功..." << std::endl;
	}

	// 2. connect 连接服务器
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		std::cout << "ERROR，连接服务器失败..." << std::endl;
	}
	else
	{
		std::cout << "SUCCESS，连接服务器成功..." << std::endl;
	}

	// 3. 与服务器通信
	while (true)
	{	
		// 3.1 输入指令
		char cmdBuf[128] = {};
		std::cout << ">>";
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit"))
		{
			std::cout << "接收到退出指令" << std::endl;
			break;
		}
		// 3.2 发送指令
		send(_sock, cmdBuf, strlen(cmdBuf) + 1, 0);

		// 3.3 recv 接收服务器信息
		char _recvBuf[128] = {};
		int nLen = recv(_sock, _recvBuf, 128, 0);
		if (nLen <= 0)
		{
			std::cout << "未从服务器端接收到任何数据...";
		}
		else
		{
			std::cout << _recvBuf << std::endl;
		}
	}

	// 4. closesocket 关闭套接字
	closesocket(_sock);

	// -----------------------------
	// 清除Windows socket 环境
	WSACleanup();
}