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

XUdp::~XUdp() //����ָ���ַ��Ҫ�������������� ��Ϊ�������XUdp����ջ�е���ʽ ��ֵ������һ�� ��close�ͻ����
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
	
	//�˿�
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port); //�����ֽ���ת���������ֽ��� ��Ϊ��ͬ����ϵͳ������bigendian ���� littleendian
	saddr.sin_addr.s_addr = htonl(0);

	if (bind(_sock, (sockaddr*)&saddr, sizeof(saddr)) != 0) { //��=0����ʧ��
		printf("bind port %d failed\n", port);
		return false;
	}
	printf("bind port %d success\n", port);
	listen(_sock, 10); //��ʼ�������� //���socket��ר�������������ӵ�

	return true;
}
int XUdp::Receive(char* buffer, int buffersize) //��Ҫ���� ƿ��������
{
	if (_sock <= 0) return 0;
	if (_addr == 0) {
		_addr = new sockaddr_in();
	}
	socklen_t len = sizeof(sockaddr_in); //ʹ�ýṹ��Ĵ�С
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
