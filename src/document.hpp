#ifndef _SEARCH_DOCUMENT_HPP_
#define _SEARCH_DOCUMENT_HPP_

#include "search.hpp"

#include "leveldb/db.h"
#include "db-wrapper.hpp"

namespace search {

class Document : public DBWrapper
{
public:
    static char DOCUMENT_DIR[];
    static char COUNT_KEY[];

private:
    inline Slice
    docid2slice(const docid_t &id)
    { return Slice(reinterpret_cast<const char *>(&id), sizeof(id)); }

    inline docid_t
    slice2docid(Slice s)
    { return *reinterpret_cast<const docid_t *>(s.data()); }

    inline Slice
    getCountSlice() const
    { return Slice(COUNT_KEY, sizeof(count_t)); }

    inline void
    setCount(count_t count)
    { put(getCountSlice(), Slice(reinterpret_cast<const char *>(&count), sizeof(count_t))); }

public:
    struct DocumentStruct
    {
        count_t word_count;
        char attr[0];
    };

    Document(std::string path)
        : DBWrapper((path.back() == '/' ? path : (path + "/")) + DOCUMENT_DIR)
    { }

    virtual ~Document() = default;

    inline count_t
    getCount()
    {
        auto value = get(getCountSlice());
        if (!value.length()) {
            return 0;
        }
        return *reinterpret_cast<const count_t*>(value.data());
    }

    inline docid_t
    addDocument(count_t word_count, std::string attr)
    {
        docid_t ret = getCount() + 1;
        attr = std::string(reinterpret_cast<const char *>(&word_count), sizeof(word_count)) + attr;
        attr.push_back('\0');

        put(docid2slice(ret), attr);
        setCount(ret);
        return ret;
    }

    inline std::string
    getDocument(docid_t id)
    { return checkedGet(docid2slice(id)); }

    inline std::string
    getDocumentAttr(docid_t id)
    {
        auto ret = getDocument(id);
        return ret.substr(sizeof(count_t), ret.length() - sizeof(count_t) - 1);
    }

    inline count_t
    getDocumentWordCount(docid_t id)
    {
        auto it = _iterator();
        it->Seek(docid2slice(id));
        count_t ret = *reinterpret_cast<const count_t*>(it->value().data());
        delete it;
        return ret;
    }

    inline void
    eraseDocument(docid_t id)
    {
        erase(docid2slice(id));
        setCount(getCount() - 1);
    }
};

}

#endif
