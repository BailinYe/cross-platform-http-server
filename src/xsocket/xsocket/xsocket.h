#ifndef XSOCKET_H
#define XSCOKET_H

#ifdef _WIN64

#ifdef XSOCKET_EXPORTS
#define XSOCKET_API __declspec(dllexport)
#else
#define XSOCKET_APT __declspec(dllimport)
#endif

#else //linux²»ÐèÒªºê
#define XSOCKET_API 
#endif

class XSOCKET_API xsocket {

public:
	xsocket();
};

#endif
