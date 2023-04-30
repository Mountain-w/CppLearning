#include <iostream>
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
	CMD_LOGOUT_RESULT
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
		else if (0 == strcmp(cmdBuf, "login"))
		{
			Login login;
			strcpy_s(login.userName, "Ruize");
			strcpy_s(login.passWord, "Ruize01.");
			// 向服务器发送请求命令
			send(_sock, (const char *) &login, sizeof(Login), 0);

			// 从服务器接收数据
			LoginResult loginRet = {};
			recv(_sock, (char*) &loginRet, sizeof(LoginResult), 0);

			std::cout << "登录是否成功:" << loginRet.result << std::endl;
		}
		else if (0 == strcmp(cmdBuf, "logout"))
		{
			Logout logout = {};
			strcpy_s(logout.userName, "Ruize");
			// 向服务器发送请求命令
			send(_sock, (char*)&logout, sizeof(Logout), 0);

			// 从服务器接收数据
			LogoutResult logoutRet = {};
			recv(_sock, (char*) &logoutRet, sizeof(LogoutResult), 0);

			std::cout << "登出是否成功:" << logoutRet.result << std::endl;
		}
		else
		{
			std::cout << "不支持的命令，请重新输入..." << std::endl;
		}
		
	}

	// 4. closesocket 关闭套接字
	closesocket(_sock);

	// -----------------------------
	// 清除Windows socket 环境
	WSACleanup();
}