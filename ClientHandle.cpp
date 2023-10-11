#include "ClientHandle.h"

#include <cassert>
#include <iostream>
#include <sys/socket.h>

ClientHandle::ClientHandle(DB* db, int fd)
	: _fd(fd)
	, _terminate(false)
	, _db(db)
{
	assert( !_thread.joinable());

	_thread = std::thread([this]() {threadLoop(); });
	pthread_setname_np(_thread.native_handle(), "ClientHandle");
}

ClientHandle::~ClientHandle()
{
	terminate();
	stop();
}

void ClientHandle::terminate()
{
	_terminate = true;
}

void ClientHandle::printSequence(Sequence seq, int index)
{
	// проверка на то, можем ли мы сделать следующий шаг
	if (std::numeric_limits<uint64_t>::max() - seq.offset < seq.shift * seq.iter)
	{
		//reset
		_db->setSequence(_fd, index,  seq.offset, seq.shift);
		std::cout << seq.offset << " ";
		return;
	}

	std::cout << seq.offset + seq.shift * seq.iter;
}

void ClientHandle::threadLoop()
{
	while (!_terminate)
	{
		std::string message = readMessage();
		
		if (message == "export seq")
		{
			//start loop
			while(true)
			{
				Record record = _db->getSequence(_fd);

				printSequence(record.first, 1);
				printSequence(record.second, 2);
				printSequence(record.third, 3);
				std::cout << std::endl;
				//sendMessage("Thank you for your message " + message);
				if (_terminate)
					return;
			}
		}

		char number;
		uint64_t offset, shift;
		if (sscanf(message.c_str(), "seq%c %llu %llu", &number, &offset, &shift) != 3)
			continue;

		if (number == '1' || number == '2' || number == '3')
		{
			_db->setSequence(_fd, number - '0', offset, shift);
		}

        //std::cout << "Received message: " << message << std::endl;
	}
}

std::string ClientHandle::readMessage()
{
	std::string buffer(1024, '\0');

	int read_bytes = recv(_fd, buffer.data(), buffer.size(), 0);
	if (read_bytes < 1)
	{
		std::cout << "Error in read message \t|\t read bytes = " << read_bytes << std::endl;
        return "";
	}
	return buffer;
}

void ClientHandle::sendMessage(const std::string& message) {
    int n = send(_fd, message.c_str(), message.size(), MSG_NOSIGNAL);
    if (n != static_cast<int>(message.size())) {
                std::cout << message << "\n";
        std::cout << "Error while sending message, message size: " 
				  << message.size() << " bytes sent: " << std::endl;
        terminate();
    }
}

void ClientHandle::stop()
{
    if (_thread.joinable()) {
        _thread.join();
    }
}

