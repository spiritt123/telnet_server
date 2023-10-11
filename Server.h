#pragma once

#include <thread>
#include <string>
#include <vector>
#include <unordered_map>

#include "ClientHandle.h"
#include "DB.h"

class Server
{
public:
	Server(std::string ip_address = "127.0.0.1", unsigned int port = 1234);
	~Server();

	void start();
	void stop();
	void join();

private:
	std::thread _thread;
	int _event_fd;
	std::string _ip_address;
	unsigned int _port;
	int _sock_fd;
	int _client_count;

	std::vector<struct pollfd> _fds;
	std::unordered_map<int, ClientHandle> _clients;

	DB _db;

private:
	void threadLoop();

	bool initSocket();
	void updateClientsState();
	void eraseFDByIndex(size_t index);
	void clear();
};

