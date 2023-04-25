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
	char _recvBuf[128] = {};
	while (true)
	{
		// 5.1 ���տͻ��˷��͵�����
		int nLen = recv(_cSock, _recvBuf, 128, 0);
		if (nLen <= 0)
		{
			std::cout << "�ͻ����Ѿ��˳����������" << std::endl;
			break;
		}
		std::cout << "�ӿͻ��˽��յ�:" << _recvBuf << std::endl;
		// 5.2 ��������
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
			char msgBuf[] = "δ֪������";
			send(_cSock, msgBuf, strlen(msgBuf) + 1, 0);
		}
	}

	// 6. closesocket �ر��׽���
	closesocket(_sock);

	// -----------------------------
	// ���Windows socket ����
	WSACleanup();
	std::cout << "hello" << std::endl;
}