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

int XTcp::Receive(char* buffer, int bufferSize) 
{
	return recv(_sock, buffer, bufferSize, 0); //��װ����Ҫ��ֵ
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

//�������� ���������ɿͻ���������
XTcp XTcp::Accept()
{	
		XTcp tcp;
		sockaddr_in caddr;
		socklen_t len{ sizeof(caddr) };

		int client = accept(_sock, (sockaddr*)&caddr, &len); //����ɹ��᷵��һ���µ�socket ����������ͻ��˵���ͨ�� //
		if (client <= 0) {
			return tcp;
		}

		printf("[%d]", _sock);
		printf("client name: %d \n", client);

		char* ip = inet_ntoa(caddr.sin_addr); //��IP��ַת����һ���ɶ�����ʽ
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
	saddr.sin_addr.s_addr = inet_addr(ip); //�ַ�������ת�����������͵�ip��ַ
	SetBlock(false);
	fd_set set; //�ļ����������
	if (connect(_sock, (sockaddr*)&saddr, sizeof(saddr)) != 0) { //1s�����û�����ӳɹ�û���ļ����Կɶ����߿�д��return false
		FD_ZERO(&set);
		FD_SET(_sock, &set); //��sock���뵽�ļ�������� _sockҲ��һ���ļ���� sock��������� ��linux���ж����ļ�
		timeval tm;
		tm.tv_sec = 0;
		tm.tv_usec = timeoutms * 1000;
		if (select(_sock + 1, 0, &set, 0, &tm) <= 0) { //��select�������Ƿ����ļ��ɶ����߿�д //windows���п��Բ��� //��������Ϊselect���Լ�������ļ���� ֻҪ��һ����д�ͷ���
			printf("Connect timeout or error\n");
			printf("connect %s: %d failed :%s\n", ip, port, strerror(errno)); //connectʧ�ܻ�Ѵ�����Ϣд��error number�� ����ͨ��strerror�Ѵ�����Ϣת�����ַ���
			return false;
		}
	}
	SetBlock(true); //���ӳɹ�֮����������Ȼʹ������ģʽ
	printf("connect %s: %d success!\n", ip, port); //connectʧ�ܻ�Ѵ�����Ϣд��error number�� ����ͨ��strerror�Ѵ�����Ϣת�����ַ���
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
	if (isblock) { //linux��������˿�û�б����� ��ô���ӻ����̷���
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
