#include <stdlib.h>
#include "XTcp.h"
#include <thread>
#include <string.h>
#include <string>
#include <regex>
#include <signal.h>
#include "XHttpServer.h"

int main(int argc, char* argv[]) {
	signal(SIGINT, SIG_IGN);

	unsigned short port = 8080;
	if (argc > 1) {
		port = atoi(argv[1]);
	}
	XHttpServer server;
	server.Start(port);
	getchar();	
	return 0;
}