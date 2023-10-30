#include "ClientHandle.h"

#include <cassert>
#include <sstream>
#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <regex>
#include <unistd.h>

ClientHandle::ClientHandle(int fd)
    : _fd(fd)
{
    for (auto seq : _records)
        seq.reset();

}

ClientHandle::~ClientHandle()
{
	std::cout << "Close thread with fd = " << _fd << std::endl;
    close(_fd);
}

std::string ClientHandle::convertSequenceToString(Sequence seq)
{
    // максимальная длина числа uint64_t = 20 символов.
    // 20 символов на число + 1 символ на пробел * 3 последовательности + 1 байт(конец строки)
    std::string str(21, ' ');

    sprintf(str.data(), "%20llu", seq.offset + seq.shift * seq.iter);

    return str;
}

void ClientHandle::threadLoop()
{
    while (true)
    {
		std::optional<std::string> message = readMessage();
		if (!message)
			break; // выход из-за ошибки при передачи сообщения
        
        std::string prefix = "export seq";
        if (message->rfind(prefix, 0) == 0) 
        {
            sendLoop();
            return;
        }

        // проверка на то, чтобы строка имела формат: 
        // слово(seq)+число(номер очереди) + число(начало последовательности) + число(шаг)
        static const std::regex r(R"(\w{3}+\d+\ +\d+\ +\d)");
        std::smatch m;
        if (!std::regex_search(*message, m, r))
            continue;

        char number;
        uint64_t offset, shift;
        if (sscanf(message->c_str(), "seq%c %lu %lu", &number, &offset, &shift) != 3)
            continue;

        if (number < '1' || number > '3')
            continue;

		if (offset == 0 || shift == 0)
			continue;

        Sequence seq;
        seq.offset = offset;
        seq.shift  = shift;
        seq.iter   = 0;
        
        // вычитаем единицу для того, что узнать индекс последовательности
        int index = number - '1';
        _records[index].emplace(seq);
    }
}

std::optional<std::string> ClientHandle::readMessage()
{
    const unsigned int MAX_BUF_LENGTH = 4096;
    std::vector<char> buffer(MAX_BUF_LENGTH);
    std::string rcv;

    int bytesReceived = 0;
    bytesReceived = recv(_fd, &buffer[0], buffer.size(),  0);
    
    if ( bytesReceived == -1  || bytesReceived == 0) 
    {
        return std::nullopt;
    } 
    else 
    {
        rcv.append( buffer.cbegin(), buffer.cend() );
    }

    return std::make_optional<std::string>(rcv);
}

bool ClientHandle::sendMessage(const std::string& message) 
{
    int n = send(_fd, message.c_str(), message.size(), MSG_NOSIGNAL);

    if (n != static_cast<int>(message.size()))
        return false;

    return true;
}

void ClientHandle::sendLoop()
{
    while (true)
    {
        std::string result = "|";

        for (size_t i = 0; i < _records.size(); ++i)
        {
            if (_records[i].has_value())
            {
                result += convertSequenceToString(_records[i].value()) + "|";
                _records[i]->nextStep();
            }
            else
            {
                // есть последовательность не была задана или 
                // при установки последовательности была допущена ошибка,
                // то заполняем колонку значения пустыми символами
                result += std::string(20, ' ') + "|";
            }
        }
        result += "\n";
        if (!sendMessage(result))
            return;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}
