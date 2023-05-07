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

	// 5.1 ���տͻ��˷��͵�����
	int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
	if (nLen <= 0)
	{
		std::cout << "��������Ͽ����ӣ��������" << std::endl;
		return -1;
	}
	switch (header.cmd)
	{
	case CMD_NEW_USER_JOIN:
	{
		NewUserJoin newUserJoin = {};
		recv(_cSock, (char*)&newUserJoin + sizeof(DataHeader), newUserJoin.dataLength - sizeof(DataHeader), 0);
		std::cout << "�¿ͻ���<" << newUserJoin.sock << "> ����" << std::endl;
	}
	break;
	case CMD_LOGIN_RESULT:
	{
		LoginResult loginResult = {};
		recv(_cSock, (char*)&loginResult + sizeof(DataHeader), loginResult.dataLength - sizeof(DataHeader), 0);
		std::cout << "��¼״̬��" << loginResult.result << std::endl;
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		LogoutResult logoutResult = {};
		recv(_cSock, (char*)&logoutResult + sizeof(DataHeader), logoutResult.dataLength - sizeof(DataHeader), 0);
		std::cout << "�ǳ�״̬��" << logoutResult.result << std::endl;
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
			std::cout << "�˳�cmd�߳�" << std::endl;
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
			std::cout << "��֧�ֵ�����" << std::endl;
		}
	}
}

int main()
{
	// ���� Windows socket 2.x ����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	// -----------------------------
	// 1. ����һ��socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock)
	{
		std::cout << "ERROR�������׽���ʧ��..." << std::endl;
	}
	else
	{
		std::cout << "SUCCESS�������׽��ֳɹ�..." << std::endl;
	}

	// 2. connect ���ӷ�����
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == ret)
	{
		std::cout << "ERROR�����ӷ�����ʧ��..." << std::endl;
	}
	else
	{
		std::cout << "SUCCESS�����ӷ������ɹ�..." << std::endl;
	}
	// �����߳�
	std::thread t1(cmdThread, _sock);
	t1.detach();
	// 3. �������ͨ��
	while (g_bRunt)
	{	
		fd_set fdReads;
		FD_ZERO(&fdReads);
		FD_SET(_sock, &fdReads);
		timeval time = { 0, 0 };
		int ret = select(0, &fdReads, nullptr, nullptr, &time);
		if (ret < 0)
		{
			std::cout << "select �������" << std::endl;
			break;
		}
		if (FD_ISSET(_sock, &fdReads))
		{
			FD_CLR(_sock, &fdReads);
			if (-1 == processor(_sock))
			{
				std::cout << "select �������" << std::endl;
				break;
			}
		}
	}

	// 4. closesocket �ر��׽���
	closesocket(_sock);

	// -----------------------------
	// ���Windows socket ����
	WSACleanup();
}