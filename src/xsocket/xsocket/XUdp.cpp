#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#ifdef _WIN64
#include <windows.h>
#define socklen_t int 
#else
//for linux
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h> 
#define closesocket close
#endif

#include "XUdp.h"

XUdp::XUdp() {
	#ifdef _WIN64
		static bool first = true;
		if (first) {
			WSADATA ws;
			WSAStartup(MAKEWORD(2, 2), &ws);
			first = false;
		}
	#endif

		memset(_ip, 0, sizeof(_ip));
}

XUdp::~XUdp() //清理指针地址不要在析构函数中做 因为如果想用XUdp这种栈中的形式 赋值会销毁一次 再close就会出错
{
}

int XUdp::CreateSocket()
{
	_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (_sock < 0) {
		printf("Create socket failed");
		return -1;
	}
	return _sock;
}
bool XUdp::Bind(unsigned short port)
{
	if (_sock <= 0) {
		CreateSocket();
	}
	
	//端口
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port); //本地字节序转换成网络字节序 因为不同主机系统可能是bigendian 或者 littleendian
	saddr.sin_addr.s_addr = htonl(0);

	if (bind(_sock, (sockaddr*)&saddr, sizeof(saddr)) != 0) { //不=0返回失败
		printf("bind port %d failed\n", port);
		return false;
	}
	printf("bind port %d success\n", port);
	listen(_sock, 10); //开始接收连接 //这个socket是专门用来建立连接的

	return true;
}
int XUdp::Receive(char* buffer, int buffersize) //需要考虑 瓶颈在哪里
{
	if (_sock <= 0) return 0;
	if (_addr == 0) {
		_addr = new sockaddr_in();
	}
	socklen_t len = sizeof(sockaddr_in); //使用结构体的大小
	int re = recvfrom(_sock, buffer, buffersize, 0, (sockaddr*)&_addr, &len);

	return re ;
}
void XUdp::Close()
{
	if (_sock <= 0) return;
	closesocket(_sock);
	if (_addr) {
		delete _addr;
	}
	_addr = 0;
	_sock = 0;
}
