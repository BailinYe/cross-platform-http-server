#include "XHttpClient.h"
#include <thread>
XHttpClient::XHttpClient()
{
}
XHttpClient::~XHttpClient()
{
}
bool XHttpClient::Start(XTcp client)
{
	this->client = client;
	std::thread sth(&XHttpClient::Main, this);
	sth.detach();
	return true;
}

void XHttpClient::Main()
{
	char buffer[10240] = { 0 };
	for (;;) {
		
		int len = client.Receive(buffer, sizeof(buffer) - 1);
		if (len <= 0) {
			break;
		}
		buffer[len] = '\0'; 
		if (!res.SetRequest(buffer)) {
			break;
		}
		std::string head = res.GetHead();
		if (client.Send(head.c_str(), head.size()) <= 0) {
			break;
		}
		int size = sizeof(buffer);
		bool error = false;
		for (;;) {
			int len = res.Read(buffer, size);
			if (len < 0) {
				error = true;
				break;
			}
			if (len == 0) break;
			if (client.Send(buffer, len) <= 0) {
				error = true;
				break;
			}
		}
	}
	client.Close();
	delete this;
}
