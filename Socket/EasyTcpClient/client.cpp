#include <iostream>
#include <thread>
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 0
#include <Windows.h>
#include <WinSock2.h>


enum CMD
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN
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
struct Login : public DataHeader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
};

struct LoginResult : public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
	}
	int result;
};

struct Logout : public DataHeader
{
	Logout()
	{
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

struct LogoutResult : public DataHeader
{
	LogoutResult()
	{
		dataLength = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
	}
	int result;
};

struct NewUserJoin : public DataHeader
{
	NewUserJoin()
	{
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		sock = 0;
	}
	int sock;
};

int processor(SOCKET _cSock)
{
	DataHeader header = {};

	// 5.1 接收客户端发送的数据
	int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
	if (nLen <= 0)
	{
		std::cout << "与服务器断开连接，任务结束" << std::endl;
		return -1;
	}
	switch (header.cmd)
	{
	case CMD_NEW_USER_JOIN:
	{
		NewUserJoin newUserJoin = {};
		recv(_cSock, (char*)&newUserJoin + sizeof(DataHeader), newUserJoin.dataLength - sizeof(DataHeader), 0);
		std::cout << "新客户端<" << newUserJoin.sock << "> 加入" << std::endl;
	}
	break;
	case CMD_LOGIN_RESULT:
	{
		LoginResult loginResult = {};
		recv(_cSock, (char*)&loginResult + sizeof(DataHeader), loginResult.dataLength - sizeof(DataHeader), 0);
		std::cout << "登录状态：" << loginResult.result << std::endl;
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		LogoutResult logoutResult = {};
		recv(_cSock, (char*)&logoutResult + sizeof(DataHeader), logoutResult.dataLength - sizeof(DataHeader), 0);
		std::cout << "登出状态：" << logoutResult.result << std::endl;
	}
	break;
	}
}

bool g_bRunt = true;
void cmdThread(SOCKET _sock)
{
	while (true)
	{
		char cmdBuf[256] = {};
		std::cout << ">>>";
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit"))
		{
			std::cout << "退出cmd线程" << std::endl;
			g_bRunt = false;
			break;
		}
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy_s(login.userName, "ruize");
			strcpy_s(login.passWord, "ruize01.");
			send(_sock, (const char*)&login, login.dataLength, 0);
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout;
			strcpy_s(logout.userName, "ruize");
			send(_sock, (const char*)&logout, logout.dataLength, 0);
		}
		else {
			std::cout << "不支持的命令" << std::endl;
		}
	}
}

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
	// 启动线程
	std::thread t1(cmdThread, _sock);
	t1.detach();
	// 3. 与服务器通信
	while (g_bRunt)
	{	
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);
		timeval time = { 0, 0 };
		int ret = select(0, &fdReads, nullptr, nullptr, &time);
		if (ret < 0)
		{
			std::cout << "select 任务结束" << std::endl;
			break;
		}
		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock, &fdReads);
			if (-1 == processor(_sock))
			{
				std::cout << "select 任务结束" << std::endl;
				break;
			}
		}
	}

	// 4. closesocket 关闭套接字
	closesocket(_sock);

	// -----------------------------
	// 清除Windows socket 环境
	WSACleanup();
}