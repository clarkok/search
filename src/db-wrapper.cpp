#include <iostream>

#include "db-wrapper.hpp"

using namespace search;

#define checkStatus()                                           \
    do {                                                        \
        if (!last_status.ok()) {                                \
            std::cerr << last_status.ToString() << std::endl;   \
            assert(last_status.ok());                           \
        }                                                       \
    } while (false)

DBWrapper::DBWrapper(std::string db_path)
{
    assert(db_path.length());

    leveldb::Options options;
    options.create_if_missing = true;

    last_status = leveldb::DB::Open(options, db_path, &db);
    checkStatus();
}

DBWrapper::~DBWrapper()
{ delete db; }

std::string
DBWrapper::checkedGet(Slice key)
{
    auto ret = get(key);
    checkStatus();
    return ret;
}

std::string
DBWrapper::get(Slice key)
{
    std::string ret;
    db->Get(leveldb::ReadOptions(), key, &ret);
    return ret;
}

void
DBWrapper::put(Slice key, Slice value)
{
    db->Put(leveldb::WriteOptions(), key, value);
    checkStatus();
}

void
DBWrapper::erase(Slice key)
{
    db->Delete(leveldb::WriteOptions(), key);
    checkStatus();
}

DBWrapper::Iterator *
DBWrapper::_iterator()
{ return db->NewIterator(leveldb::ReadOptions()); }

DBWrapper::Iterator *
DBWrapper::firstIterator()
{
    auto ret = _iterator();
    ret->SeekToFirst();
    return ret;
}

DBWrapper::Iterator *
DBWrapper::lastIterator()
{
    auto ret = _iterator();
    ret->SeekToLast();
    return ret;
}
