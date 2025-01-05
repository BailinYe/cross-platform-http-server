#include "XHttpServer.h"
#include "XHttpClient.h"
#include <thread>
bool XHttpServer::Start(unsigned short port) {
	isexit = false;
	server.CreateSocket();
	if (!server.Bind(port)) return false;
	std::thread sth(&XHttpServer::Main, this);
	sth.detach();

	return true;
}
XHttpServer::XHttpServer() {

}

XHttpServer::~XHttpServer() {

}

void XHttpServer::Main()
{
	while (!isexit) {
		XTcp client = server.Accept();
		if (client._sock <= 0) continue;
		XHttpClient* th = new XHttpClient();
		th->Start(client);
	}
}

void XHttpServer::Stop()
{
	isexit = true;
}
