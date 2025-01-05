#ifndef XUDP_H //ͬһ��cpp�ļ�
#define XUDP_H

#include <string>

#ifdef _WIN64

#ifdef XSOCKET_EXPORTS
#define XSOCKET_API __declspec(dllexport)
#else
#define XSOCKET_APT __declspec(dllimport)
#endif

#else //linux����Ҫ��
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
	int Receive(char* buffer, int buffersize); //�����������ڲ����з�װ �ٴ�ip��ַ���س���
	char _ip[16]; //���ʺ�receiveһ��Ҫ��һ���߳��� ��Ϊreceive��ʱ����ܻ�������ֵ
	unsigned short port = 0;
protected:
	int _sock = 0;
private:
	void* _addr = NULL;
};

#endif
