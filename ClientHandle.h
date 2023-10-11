#pragma once

#include <string>
#include <thread>

#include "DB.h"

class ClientHandle
{
public:
	explicit ClientHandle(int fd);
	ClientHandle() = default;
	void setDB(DB *db);
	~ClientHandle();

	void terminate();

private:
	std::string convertSequenceToString(Sequence seq, int index);
	void threadLoop();
	std::string readMessage();
	void sendMessage(const std::string &message);
	void stop();

private:
	int _fd;
	bool _terminate;
	std::thread _thread;

	DB *_db;
};

