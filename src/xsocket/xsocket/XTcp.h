#ifndef XTCP_H //ͬһ��cpp�ļ�
#define XTCP_H

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


class XSOCKET_API XTcp
{
public:
	int	CreateSocket();
	bool Bind(unsigned short port);

	int Receive(char* buffer, int bufferSize);
	int Send(const char* buffer, int sendSize);
	XTcp Accept();
	void Close();
	bool Connect(const char* ip, unsigned short port, int timeoutms=1000); //��������ⳬʱ
	bool SetBlock(bool isblock); //������ϣ����������������ʱ�Ƿ������� ��������ʱ��������
	XTcp();
	int _sock = 0;
	unsigned short _port = 0;
	char _ip[16]; //stringû���Զ����� DLL �ӿ�
	
};

#endif
