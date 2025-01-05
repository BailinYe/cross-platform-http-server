#ifndef XUDP_H //同一个cpp文件
#define XUDP_H

#include <string>

#ifdef _WIN64

#ifdef XSOCKET_EXPORTS
#define XSOCKET_API __declspec(dllexport)
#else
#define XSOCKET_APT __declspec(dllimport)
#endif

#else //linux不需要宏
#define XSOCKET_API 
#endif

class XSOCKET_API XUdp
{
public:
	XUdp();
	~XUdp();
	int CreateSocket();
	void Close();	
	bool Bind(unsigned short port);
	int Receive(char* buffer, int buffersize); //其他参数由内部进行封装 再从ip地址返回出来
	char _ip[16]; //访问和receive一定要在一个线程中 因为receive的时候可能会更改这个值
	unsigned short port = 0;
protected:
	int _sock = 0;
private:
	void* _addr = NULL;
};

#endif
