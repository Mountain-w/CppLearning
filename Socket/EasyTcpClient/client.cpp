#include <iostream>
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS 0
#include <Windows.h>
#include <WinSock2.h>


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

	// 3. �������ͨ��
	while (true)
	{	
		// 3.1 ����ָ��
		char cmdBuf[128] = {};
		std::cout << ">>";
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit"))
		{
			std::cout << "���յ��˳�ָ��" << std::endl;
			break;
		}
		// 3.2 ����ָ��
		send(_sock, cmdBuf, strlen(cmdBuf) + 1, 0);

		// 3.3 recv ���շ�������Ϣ
		char _recvBuf[128] = {};
		int nLen = recv(_sock, _recvBuf, 128, 0);
		if (nLen <= 0)
		{
			std::cout << "δ�ӷ������˽��յ��κ�����...";
		}
		else
		{
			std::cout << _recvBuf << std::endl;
		}
	}

	// 4. closesocket �ر��׽���
	closesocket(_sock);

	// -----------------------------
	// ���Windows socket ����
	WSACleanup();
}