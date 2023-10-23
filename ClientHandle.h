#pragma once

#include <string>
#include <thread>
#include <array>
#include <optional>

struct Sequence
{
    uint64_t offset;
    uint64_t shift;
    uint64_t iter;

    void nextStep()
    {
        ++iter;
        
        //проверим можем ли мы сделать сдвиг от последовательности
        if ((uint64_t)(-1) - shift * (iter - 1) < shift)
        {
            iter = 0;
            return;
        }
        
        //обработка случая суммы
        if ((uint64_t)(-1) - offset < shift * iter)
        {
            iter = 0;
            return;
        }
    }
};

class ClientHandle
{
public:
    explicit ClientHandle(int fd);
    ClientHandle() = default;
    ~ClientHandle();
    void threadLoop();

private:
    std::string convertSequenceToString(Sequence seq);
	std::optional<std::string> readMessage();
    void sendLoop();
    bool sendMessage(const std::string &message);

private:
    int _fd;
    std::array<std::optional<Sequence>, 3> _records;
};

