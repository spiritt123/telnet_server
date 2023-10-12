#include "Server.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <cassert>

#include <sys/eventfd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

Server::Server(std::string ip_address, unsigned int port)
	: _event_fd(-1)
	, _ip_address(ip_address)
	, _port(port)
	, _client_count(50)
{
}

Server::~Server()
{
	stop();
	if (_event_fd != -1)
	{
		close(_event_fd);
	}
	clear();
}

void Server::start()
{
	assert(!_thread.joinable());

	if (_event_fd != -1)
	{
		close(_event_fd);
	}

	_event_fd = eventfd(0, 0);
	if (_event_fd == -1)
	{
		std::cout << "eventfd() => -1 \t|\t errno = " << errno << std::endl;
		return;
	}

	_thread = std::thread([this]() { threadLoop(); });

	pthread_setname_np(_thread.native_handle(), "Server");
}

void Server::stop()
{
	uint64_t one = 1;
	auto ret = write(_event_fd, &one, 8);
	if (ret == -1)
	{
		std::cout << "write => -1 \t|\t errno = " << errno << std::endl;
	}
}

void Server::join()
{
	if (_thread.joinable())
	{
		_thread.join();
	}
}

void Server::threadLoop()
{
	if (!initSocket())
		return;

	while (true)
	{
		const int TIMEOUT = 1000;
		int n = poll(_fds.data(), _fds.size(), TIMEOUT);
		if (n == -1 && errno != ETIMEDOUT && errno != EINTR)
		{
			std::cout << "pool() => -1 \t|\t erron = " << errno << std::endl;
			break;
		}

		if (n <= 0) continue;
		
		if (_fds[0].revents) 
		{ 
            std::cout << "Received stop request" << std::endl;
            break;
        } 
		if (_fds[1].revents) 
		{
			int client_fd = accept(_sock_fd, NULL, NULL);
			std::cout << "New connection" << std::endl;
			if (client_fd != -1) {
				_fds.push_back(pollfd{client_fd, POLLIN, 0});
				_clients.emplace(client_fd, client_fd);
				_clients[client_fd].setDB(&_db);
			} else {
				std::cout << "accept => -1 \t|\t errno = " << errno << std::endl;
			}
            _fds[1].revents = 0;
			_db.createClientInfo(client_fd);
        }

		updateClientsState();
	}

	clear();
}


bool Server::initSocket()
{
	std::cout << "Listen on port " << _port << std::endl;
	if ((_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cout << "socket() => -1 \t|\t errno" << errno << std::endl;
		return false;
	}

	int user_address = 1;
	if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &user_address, sizeof(int)) == -1)
	{
		std::cout << "setsockopt() => -1 \t|\t errno" << errno << std::endl;
	}

	struct sockaddr_in server_address = {0, 0, 0, 0};
	server_address.sin_family = AF_INET;
	int inet_address = inet_aton(_ip_address.c_str(), &server_address.sin_addr);
	//if (inet_address) server_address.sin_addr.s_addr = INADDR_ANY;
	if (!inet_address) server_address.sin_addr.s_addr = INADDR_ANY;
	//server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(_port);

	if (bind(_sock_fd, (const struct sockaddr*)&server_address, sizeof(server_address)) == -1)
	{
		std::cout << "bind() => -1 \t|\t errno" << errno << std::endl;
		close(_sock_fd);
		return false;
	}

	listen(_sock_fd, _client_count);
	
	_fds.push_back( {_event_fd, POLLIN, 0} );
	_fds.push_back( {_sock_fd, POLLIN, 0} );

	return true;
}


void Server::updateClientsState()
{
	char c;
	for (size_t i = 2; i < _fds.size(); ++i)
	{
		if (!_fds[i].revents) continue;
		if (recv(_fds[i].fd, &c, 1, MSG_PEEK | MSG_DONTWAIT) != 0) continue;

		std::cout << "Client disconnected" << std::endl;
		eraseFDByIndex(i);
		--i; // коррекция индекса при удалении текущего элемента.
	}
}

void Server::eraseFDByIndex(size_t index)
{
	_db.destroyClientInfo(index);
	_clients.erase(_fds[index].fd);
	close(_fds[index].fd);
	_fds.erase(_fds.begin() + index);
}

void Server::clear()
{
    for (auto it = _fds.begin() + 1; it != _fds.end(); ++it) 
	{
        close(it->fd);
        if (_clients.find(it->fd) != _clients.end()) 
		{
            _clients.at(it->fd).terminate();
        }
    }
	_fds.clear();
}


