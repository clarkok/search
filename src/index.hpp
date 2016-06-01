#ifndef _SEARCH_INDEX_HPP_
#define _SEARCH_INDEX_HPP_

#include <cassert>
#include <iostream>
#include <algorithm>
#include "search.hpp"

#include "leveldb/db.h"
#include "word-list.hpp"
#include "db-wrapper.hpp"

namespace search {

class Index : public DBWrapper
{
    static char INDEX_DIR[];

public:
    struct IndexItem
    {
        docid_t id;
        count_t count;
    };

    struct IndexRow
    {
        count_t total_count;
        IndexItem items[0];
    };

    Index(std::string path)
        : DBWrapper((path.back() == '/' ? path : (path + "/")) + INDEX_DIR)
    { }

    virtual ~Index() = default;

    inline void
    addIndex(WordList *list, docid_t doc, count_t count)
    {
        auto key = list->getSlice();
        IndexItem item = { doc, count };

        auto original = get(key);
        if (!original.length()) {
            count_t total_count = count;
            original = std::string(reinterpret_cast<const char *>(&total_count), sizeof(total_count));
        }
        else {
            *reinterpret_cast<count_t*>(const_cast<char *>(original.data())) += count;
        }

        put(
            key,
            original + std::string(reinterpret_cast<const char *>(&item), sizeof(item))
        );
    }

    inline std::string 
    getIndex(WordList *list)
    { return checkedGet(list->getSlice()); }

    inline count_t
    getIndexWordCount(WordListWrapper list)
    {
        count_t ret = 0;
        auto it = _iterator();
        it->Seek(list.getRaw()->getSlice());
        if (!it->Valid()) {
            ret = 0;
        }
        else if (list != WordListWrapper(it->key())) {
            ret = 0;
        }
        else {
            ret = *reinterpret_cast<const count_t *>(it->value().data());
        }
        delete it;
        return ret;
    }

    inline auto
    getIterator(leveldb::Slice slice) -> decltype(_iterator())
    {
        auto it = _iterator();
        it->Seek(slice);
        return it;
    }

    static inline count_t
    rowSize(const std::string &row)
    { return (row.length() - sizeof(IndexRow)) / sizeof(IndexItem); }

    inline void
    dumpIndex(std::ostream &os)
    {
        for (
            auto it = firstIterator();
            it->Valid();
            it->Next()
        ) {
            auto key = WordListWrapper(it->key());
            for (
                auto word_iter = key.begin();
                word_iter != key.end();
                ++word_iter
            ) {
                os << *word_iter << "\t";
            }
            os << ":\n";

            auto value = reinterpret_cast<const IndexRow *>(it->value().data());
            for (
                auto i = 0u;
                i < (it->value().size() - sizeof(IndexRow)) / sizeof(IndexItem);
                ++i
            ) {
                os << "\t" << value->items[i].id << "\t" << value->items[i].count << "\n";
            }
            os << std::endl;
        }
    }
};

static_assert(sizeof(Index::IndexItem) == 8, "sizeof IndexItem should be 8");

}

#endif
