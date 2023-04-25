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
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 2. bind 绑定用于接受客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567); // host to net unsigned short
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
	{
		std::cout << "ERROR，绑定用于接受客户端连接的网络端口失败..." << std::endl;
	}
	else
	{
		std::cout << "SUCCESS，绑定用于接受客户端连接的网络端口成功..." << std::endl;
	}

	// 3. listen 监听网络端口
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		std::cout << "ERROR，监听用于接受客户端连接的网络端口失败..." << std::endl;
	}
	else
	{
		std::cout << "SUCCESS，监听用于接受客户端连接的网络端口成功..." << std::endl;
	}
	
	// 4. accept 等待客户端连接
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock)
	{
		std::cout << "ERROR，接受到无效客户端SOCKET..." << std::endl;
	}
	std::cout << "新客户端加入：IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;
	char _recvBuf[128] = {};
	while (true)
	{
		// 5.1 接收客户端发送的数据
		int nLen = recv(_cSock, _recvBuf, 128, 0);
		if (nLen <= 0)
		{
			std::cout << "客户端已经退出，任务结束" << std::endl;
			break;
		}
		std::cout << "从客户端接收到:" << _recvBuf << std::endl;
		// 5.2 处理请求
		if (0 == strcmp(_recvBuf, "getName"))
		{
			char msgBuf[] = "XiaoQiang";
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else if (0 == strcmp(_recvBuf, "getAge"))
		{
			char msgBuf[] = "80";
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
		else
		{
			char msgBuf[] = "未知的命令";
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
	}

	// 6. closesocket 关闭套接字
	closesocket(_sock);

	// -----------------------------
	// 清除Windows socket 环境
	WSACleanup();
	std::cout << "hello" << std::endl;
}