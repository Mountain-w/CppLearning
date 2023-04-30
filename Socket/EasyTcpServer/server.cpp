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
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
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
	
	// 4. accept �ȴ��ͻ�������
	sockaddr_in clientAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
	_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _cSock)
	{
		std::cout << "ERROR�����ܵ���Ч�ͻ���SOCKET..." << std::endl;
	}
	std::cout << "�¿ͻ��˼��룺IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;
	while (true)
	{
		DataHeader header = {};

		// 5.1 ���տͻ��˷��͵�����ͷ
		int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
		if (nLen <= 0)
		{
			std::cout << "�ͻ����Ѿ��˳����������" << std::endl;
			break;
		}
		std::cout << "�յ�����:" << header.cmd << ", ���ݳ���:" << header.dataLength << std::endl;
		switch (header.cmd)
		{
		case CMD_LOGIN:
		{
			Login login = {};
			recv(_cSock, (char*)&login, sizeof(Login), 0);
			// �����ж��û������Ƿ���ȷ
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

	// 6. closesocket �ر��׽���
	closesocket(_sock);

	// -----------------------------
	// ���Windows socket ����
	WSACleanup();
	std::cout << "hello" << std::endl;
}