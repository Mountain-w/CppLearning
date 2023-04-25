#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>


int main()
{
	// 启动 Windows socket 2.x 环境
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	// -----------------------------




	// -----------------------------
	// 清除Windows socket 环境
	WSACleanup();
	std::cout << "hello" << std::endl;
}