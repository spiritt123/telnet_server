#pragma once

#include <cstdint>
#include <array>
#include <unordered_map>
#include <mutex>

struct Sequence
{
	uint64_t offset;
	uint64_t shift;
	uint64_t iter;
};

struct Record
{
	Sequence first, second, third;
};

class DB
{
public:
	DB();
	~DB();

	void createClientInfo(uint64_t id);
	void destroyClientInfo(uint64_t id);
	void setSequence(uint64_t id, int number, uint64_t offset, uint64_t shift);

	Record getSequence(uint64_t id);

private:

	std::unordered_map<uint64_t, Record> _records;
	std::mutex _mtx;
};

