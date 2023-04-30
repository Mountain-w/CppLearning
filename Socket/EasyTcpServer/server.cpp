#include <iostream>
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 0
#include <Windows.h>
#include <WinSock2.h>

enum CMD
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR
};

struct DataHeader
{
	short dataLength;
	short cmd;
};

//struct DataPackage
//{
//	int age;
//	char name[32];
//};
struct Login
{
	char userName[32];
	char passWord[32];
};

struct LoginResult
{
	int result;
};

struct Logout
{
	char userName[32];
};

struct LogoutResult
{
	int result;
};

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
	while (true)
	{
		DataHeader header = {};

		// 5.1 接收客户端发送的数据头
		int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			std::cout << "客户端已经退出，任务结束" << std::endl;
			break;
		}
		std::cout << "收到命令:" << header.cmd << ", 数据长度:" << header.dataLength << std::endl;
		switch (header.cmd)
		{
		case CMD_LOGIN:
		{
			Login login = {};
			recv(_cSock, (char*)&login, sizeof(Login), 0);
			// 忽略判断用户密码是否正确
			LoginResult ret = {1};
			send(_cSock, (char*)&header, sizeof(DataHeader), 0);
			send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
		}
		break;
		case CMD_LOGOUT:
		{
			Logout logout = {};
			recv(_cSock, (char*)&logout, sizeof(Logout), 0);
			LogoutResult ret = {1};
			send(_cSock, (char*)&header, sizeof(DataHeader), 0);
			send(_cSock, (char*)&ret, sizeof(LogoutResult), 0);
		}
		break;
		default:
		{
			header.cmd = CMD_ERROR;
			header.dataLength = 0;
			send(_cSock, (char*)&header, sizeof(DataHeader), 0);
		}
		break;
		}
	}

	// 6. closesocket 关闭套接字
	closesocket(_sock);

	// -----------------------------
	// 清除Windows socket 环境
	WSACleanup();
	std::cout << "hello" << std::endl;
}