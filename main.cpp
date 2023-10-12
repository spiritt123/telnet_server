#include "Server.h"

int main(int argc, char* argv[]) 
{
	std::string ip = "127.0.0.1";
	unsigned int  port = 1234;

	if (argc == 3)
	{
		ip = std::string(argv[1]);
		port = atoi(argv[2]);
	}
    Server server(ip, port);
	server.start();
    server.join();

    return 0;
}


