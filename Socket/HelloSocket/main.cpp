#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>


int main()
{
	// ���� Windows socket 2.x ����
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	// -----------------------------




	// -----------------------------
	// ���Windows socket ����
	WSACleanup();
	std::cout << "hello" << std::endl;
}