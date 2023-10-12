
#include "DB.h"

DB::DB()
{

}

DB::~DB()
{

}

void DB::createClientInfo(uint64_t id)
{
	std::lock_guard<std::mutex> l{_mtx};
	_records[id] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
}

void DB::destroyClientInfo(uint64_t id)
{
	std::lock_guard<std::mutex> l{_mtx};
	_records.erase(id);
}


void DB::setSequence(uint64_t id, int number, uint64_t offset, uint64_t shift)
{
	// Инвариант
	if (!offset || !shift)
		return;

	std::lock_guard<std::mutex> l{_mtx};
	switch (number)
	{
	case 1:
		_records[id].first = {offset, shift, 0};
		break;
	case 2:
		_records[id].second = {offset, shift, 0};
		break;
	case 3:
		_records[id].third = {offset, shift, 0};
		break;
	}

}

void incrementRecord(Sequence &sequence)
{
	if (sequence.offset && sequence.shift)
		sequence.iter++;
}

Record DB::getSequence(uint64_t id)
{
	std::lock_guard<std::mutex> l{_mtx};

	Record &record = _records[id];
	Record result = record;

	incrementRecord(record.first);
	incrementRecord(record.second);
	incrementRecord(record.third);

	return result;
}
