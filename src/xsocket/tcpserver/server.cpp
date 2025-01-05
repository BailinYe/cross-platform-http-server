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
				client.Send(quitMessage, strlen(quitMessage) + 1); //发送三个字节也就是能发送三个字符
				break;
			}
			char sendMessage[] = "send success\n";
			int sendlen = client.Send(sendMessage, strlen(sendMessage) + 1); //发送三个字节也就是能发送三个字符


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
		//没有新的连接 会一直停在这里
		XTcp client = server.Accept();

		TcpThread* th = new TcpThread();
		th->client = client;
		std::thread sth(&TcpThread::Main, th); //用th的main方法作为该线程的入口函数
		sth.detach(); //释放主线程拥有子线程的资源 比如句柄 不让主线程控制 
	}
	server.Close();

	return 0;
}