#include <iostream>
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 0
#include <Windows.h>
#include <WinSock2.h>
#include <vector>

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
struct Login: public DataHeader
{
	Login()
	{
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
};

struct LoginResult: public DataHeader
{
	LoginResult()
	{
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
	}
	int result;
};

struct Logout: public DataHeader
{
	Logout()
	{
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

struct LogoutResult: public DataHeader
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

std::vector<SOCKET> g_clients;
int processor(SOCKET _cSock)
{
	DataHeader header = {};

	// 5.1 ���տͻ��˷��͵�����
	int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
	if (nLen <= 0)
	{
		std::cout << "�ͻ����Ѿ��˳����������" << std::endl;
		return -1;
	}
	std::cout << "�ӿͻ���<" << _cSock << ">�յ�����:" << header.cmd << ", ���ݳ���:" << header.dataLength << std::endl;
	switch (header.cmd)
	{
	case CMD_LOGIN:
	{
		Login login = {};
		recv(_cSock, (char*)&login + sizeof(DataHeader), login.dataLength - sizeof(DataHeader), 0);
		// �����ж��û������Ƿ���ȷ
		LoginResult ret;
		ret.result = 1;
		send(_cSock, (char*)&ret, sizeof(LoginResult), 0);
	}
	break;
	case CMD_LOGOUT:
	{
		Logout logout = {};
		recv(_cSock, (char*)&logout + sizeof(DataHeader), sizeof(Logout) - sizeof(DataHeader), 0);
		LogoutResult ret = {};
		ret.result = 1;
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
int main()
{
	// ���� Windows socket 2.x ����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	// -----------------------------
	// 1. ����һ��socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 2. bind �����ڽ��ܿͻ������ӵ�����˿�
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567); // host to net unsigned short
	_sin.sin_addr.S_un.S_addr = inet_addr("192.168.88.109");
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
	{
		std::cout << "ERROR�������ڽ��ܿͻ������ӵ�����˿�ʧ��..." << std::endl;
	}
	else
	{
		std::cout << "SUCCESS�������ڽ��ܿͻ������ӵ�����˿ڳɹ�..." << std::endl;
	}

	// 3. listen ��������˿�
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		std::cout << "ERROR���������ڽ��ܿͻ������ӵ�����˿�ʧ��..." << std::endl;
	}
	else
	{
		std::cout << "SUCCESS���������ڽ��ܿͻ������ӵ�����˿ڳɹ�..." << std::endl;
	}

	while (true)
	{
		// ������ socket
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;

		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);

		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);

		for (int n = (int)g_clients.size() - 1; n >= 0; n--)
		{
			FD_SET(g_clients[n], &fdRead);
		}
		// ndfs ��һ������ֵ����ָfd_set������������������socket���ķ�Χ������������
		// �������ļ����������ֵ��һ���� Windows �������������ν
		timeval time = {0, 0};
		int ret = select((int)_sock + 1, &fdRead, &fdWrite, &fdExp, &time);
		if (ret < 0)
		{
			std::cout << "select �������" << std::endl;
			break;
		}
		if (FD_ISSET(_sock, &fdRead))
		{
			FD_CLR(_sock, &fdRead);
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;
			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSock)
			{
				std::cout << "ERROR�����ܵ���Ч�ͻ���SOCKET..." << std::endl;
			}
			else
			{
				for (int n = (int)g_clients.size() - 1; n >= 0; n--)
				{
					NewUserJoin userJoin;
					userJoin.sock = (int)_cSock;
					send(g_clients[n], (const char*)&userJoin, userJoin.dataLength, 0);
				}
				g_clients.push_back(_cSock);
				std::cout << "�¿ͻ��˼��룺IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;
			}
		}
		for (size_t n = 0; n < fdRead.fd_count; n++)
		{
			if (-1 == processor(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}
	}

	for (int n = (int)g_clients.size() - 1; n >= 0; n--)
	{
		closesocket(g_clients[n]);
	}


	// 6. closesocket �ر��׽���
	closesocket(_sock);

	// -----------------------------
	// ���Windows socket ����
	WSACleanup();
	std::cout << "hello" << std::endl;
}