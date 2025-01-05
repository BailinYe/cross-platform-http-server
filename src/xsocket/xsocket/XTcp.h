#ifndef XTCP_H //同一个cpp文件
#define XTCP_H

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


class XSOCKET_API XTcp
{
public:
	int	CreateSocket();
	bool Bind(unsigned short port);

	int Receive(char* buffer, int bufferSize);
	int Send(const char* buffer, int sendSize);
	XTcp Accept();
	void Close();
	bool Connect(const char* ip, unsigned short port, int timeoutms=1000); //非阻塞检测超时
	bool SetBlock(bool isblock); //传参是希望接受数据在连接时是非阻塞的 传输数据时是阻塞的
	XTcp();
	int _sock = 0;
	unsigned short _port = 0;
	char _ip[16]; //string没有自动具有 DLL 接口
	
};

#endif
