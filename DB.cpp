
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

Record DB::getSequence(uint64_t id)
{
	std::lock_guard<std::mutex> l{_mtx};

	Record &record = _records[id];
	Record result = record;

	record.first.iter++;
	record.second.iter++;
	record.third.iter++;

	return result;
}
