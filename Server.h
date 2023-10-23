#pragma once

#include <thread>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

#include "ClientHandle.h"

class Server
{
public:
    Server(std::string ip_address = "127.0.0.1", unsigned int port = 1234);
    ~Server();

    void start();
    void stop();
    void join();

private:
    bool initSocket();
    void threadLoop();

    void startClientHandle(int);

private:
    std::mutex _mtx;
    std::thread _thread;

    int _event_fd;
    int _sock_fd;

    std::string _ip_address;
    unsigned int _port;
    int _client_count;

    std::vector<struct pollfd> _fds;
    std::unordered_map<int, ClientHandle> _clients;
};

