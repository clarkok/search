#ifndef _SEARCH_DB_WRAPPER_HPP_
#define _SEARCH_DB_WRAPPER_HPP_

#include "leveldb/db.h"

namespace search {

using Slice = leveldb::Slice;

class DBWrapper
{
    using Iterator = leveldb::Iterator;

protected:

    leveldb::DB *db;
    leveldb::Status last_status;

    std::string checkedGet(Slice key);
    std::string get(Slice key);
    void put(Slice key, Slice value);
    void erase(Slice key);
    Iterator *_iterator();
    Iterator *firstIterator();
    Iterator *lastIterator();

public:
    DBWrapper(std::string db_path);
    virtual ~DBWrapper();
};

}

#endif
