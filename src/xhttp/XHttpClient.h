#pragma once
#include "XTcp.h"
#include "XHttpResponse.h"
class XHttpClient
{
public:
	XHttpClient();
	~XHttpClient();
	bool Start(XTcp client);
	XTcp client;
	void Main();
	XHttpResponse res;
};

