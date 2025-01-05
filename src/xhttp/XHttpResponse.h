#pragma once
#include <string>
class XHttpResponse
{
public:
	bool SetRequest(std::string request);
	std::string GetHead();
	int Read(char* buffer, int bufferSize);
	XHttpResponse();
	~XHttpResponse();
private:
	int filesize = 0;
	FILE* fp = NULL;
};

