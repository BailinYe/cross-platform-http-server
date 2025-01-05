#include <stdlib.h>
#include "XTcp.h"
#include <thread>
#include <string.h>

class TcpThread
{
public:
	void Main() {

		char buffer[1024] = { 0 };

		for (;;) {
			int recvlen = client.Receive(buffer, sizeof(buffer) - 1);
			if (recvlen <= 0) break;
			buffer[recvlen] = '\0';
			if (strstr(buffer, "quit") != NULL) {
				char quitMessage[] = "quit success\n";
				client.Send(quitMessage, strlen(quitMessage) + 1); //���������ֽ�Ҳ�����ܷ��������ַ�
				break;
			}
			char sendMessage[] = "send success\n";
			int sendlen = client.Send(sendMessage, strlen(sendMessage) + 1); //���������ֽ�Ҳ�����ܷ��������ַ�


			printf("recv %s\n", buffer);
		}
		client.Close();
		delete this;
	}
	XTcp client;
};

int main(int argc, char* argv[]) {

	unsigned short port = 8080;
	if (argc > 1) {
		port = atoi(argv[1]);
	}
	XTcp server;
	server.CreateSocket();
	server.Bind(port);

	for (;;) {
		//û���µ����� ��һֱͣ������
		XTcp client = server.Accept();

		TcpThread* th = new TcpThread();
		th->client = client;
		std::thread sth(&TcpThread::Main, th); //��th��main������Ϊ���̵߳���ں���
		sth.detach(); //�ͷ����߳�ӵ�����̵߳���Դ ������ �������߳̿��� 
	}
	server.Close();

	return 0;
}