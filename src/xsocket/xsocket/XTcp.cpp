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

#include "XTcp.h"

int XTcp::CreateSocket()
{
	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock < 0) {
		printf("Create socket failed");
		return -1;
	}
	return _sock;
}

bool XTcp::Bind(unsigned short port)
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

int XTcp::Receive(char* buffer, int bufferSize) 
{
	return recv(_sock, buffer, bufferSize, 0); //封装不必要的值
}

int XTcp::Send(const char* buffer, int sendSize)
{
	int sendedSize = 0;
	while(sendedSize != sendSize) {
		int len = send(_sock, buffer + sendedSize, sendSize - sendedSize, 0);
		if (len <= 0) break;
		sendedSize += len;
	}
	return sendedSize; 
}

//接受连接 具体数据由客户端来处理
XTcp XTcp::Accept()
{	
		XTcp tcp;
		sockaddr_in caddr;
		socklen_t len{ sizeof(caddr) };

		int client = accept(_sock, (sockaddr*)&caddr, &len); //如果成功会返回一个新的socket 用来与这个客户端单独通信 //
		if (client <= 0) {
			return tcp;
		}

		printf("[%d]", _sock);
		printf("client name: %d \n", client);

		char* ip = inet_ntoa(caddr.sin_addr); //将IP地址转换成一个可读的形式
		strcpy(tcp._ip, ip);
		tcp._port = ntohs(caddr.sin_port);
		tcp._sock = client;
		printf("client ip: %s, clientport: %d\n", tcp._ip, tcp._port);

		return tcp;
}

void XTcp::Close()
{
	if (_sock <= 0) return;
	closesocket(_sock);
	_sock = 0;
}

bool XTcp::Connect(const char* ip, unsigned short port, int timeoutms)
{
	if (_sock <= 0) {
		CreateSocket();
	}
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip); //字符串类型转换成整数类型的ip地址
	SetBlock(false);
	fd_set set; //文件句柄的数组
	if (connect(_sock, (sockaddr*)&saddr, sizeof(saddr)) != 0) { //1s中如果没有连接成功没有文件可以可读或者可写就return false
		FD_ZERO(&set);
		FD_SET(_sock, &set); //把sock加入到文件句柄当中 _sock也是一个文件句柄 sock是网咯句柄 在linux当中都是文件
		timeval tm;
		tm.tv_sec = 0;
		tm.tv_usec = timeoutms * 1000;
		if (select(_sock + 1, 0, &set, 0, &tm) <= 0) { //用select来监听是否有文件可读或者可写 //windows当中可以不设 //设置是因为select可以监听多个文件句柄 只要有一个可写就返回
			printf("Connect timeout or error\n");
			printf("connect %s: %d failed :%s\n", ip, port, strerror(errno)); //connect失败会把错误信息写在error number里 我们通过strerror把错误信息转换成字符串
			return false;
		}
	}
	SetBlock(true); //连接成功之后发送数据仍然使用阻塞模式
	printf("connect %s: %d success!\n", ip, port); //connect失败会把错误信息写在error number里 我们通过strerror把错误信息转换成字符串
	return true;
}

bool XTcp::SetBlock(bool isblock)
{
	if (_sock <= 0) {
		return false;
	}
#ifdef _WIN64
	unsigned long ul = 0;
	if (!isblock) ul = 1;
	ioctlsocket(_sock, FIONBIO, &ul);//file IO NON-BLOCKING IO
#else
	int flags = fcntl(_sock, F_GETFL, 0);
	if (flags < 0) {
		return false;
	}
	if (isblock) { //linux当中如果端口没有被监听 那么连接会立刻返回
		flags = flags & ~O_NONBLOCK;
	}
	else {
		flags = flags | O_NONBLOCK;
	}
	if (fcntl(_sock, F_SETFL, flags) != 0) return false;
#endif
	return true;
}

XTcp::XTcp() {
	#ifdef _WIN64
		static bool first = true;
		if (first) {
			WSADATA ws;
			WSAStartup(MAKEWORD(2, 2), &ws);
			first = false;
		}
	#endif
}
