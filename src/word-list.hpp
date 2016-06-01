#ifndef _SEARCH_WORD_LIST_HPP_
#define _SEARCH_WORD_LIST_HPP_

#include <vector>
#include <string>
#include <iterator>
#include <memory>
#include <cstring>

#include "leveldb/slice.h"

namespace search {

struct WordList
{
    uint32_t length;
    char content[0];

    WordList() = delete;
    WordList(const WordList &) = delete;
    ~WordList() = delete;

    static WordList *fromStringVector(const std::vector<std::string> &words);
    static WordList *fromSlice(leveldb::Slice slice);
    static void free(WordList *list);

    leveldb::Slice
    getSlice() const
    { return leveldb::Slice(content, length); }
};

struct WordListIterator : std::iterator<std::input_iterator_tag, const char *>
{
    const char *ptr;

    WordListIterator(const char *ptr = nullptr)
        : ptr(ptr)
    { }

    WordListIterator(const WordListIterator &iter)
        : ptr(iter.ptr)
    { }

    bool
    operator == (const WordListIterator &iter) const
    { return ptr == iter.ptr; }

    bool
    operator != (const WordListIterator &iter) const
    { return !(this->operator==(iter)); }

    const char *
    operator *() const
    { return ptr; }

    WordListIterator &
    operator ++()
    {
        while (*ptr++);
        if (!*ptr) { ptr = nullptr; }
        return *this;
    }

    WordListIterator
    operator ++(int)
    {
        auto ret = *this;
        ++*this;
        return ret;
    }
};

inline WordListIterator
begin(const WordList &list)
{ return WordListIterator(list.content); }

inline WordListIterator
end(const WordList &list)
{ return nullptr; }

class WordListWrapper
{
    std::shared_ptr<WordList> raw;

public:
    WordListWrapper(WordList *_raw)
        : raw(_raw, WordList::free)
    { }

    WordListWrapper(const std::vector<std::string> &words)
        : WordListWrapper(WordList::fromStringVector(words))
    { }

    WordListWrapper(leveldb::Slice slice)
        : WordListWrapper(WordList::fromSlice(slice))
    { }

    WordListWrapper(const WordListWrapper &wrapper)
        : raw(wrapper.raw)
    { }

    ~WordListWrapper() = default;

    inline WordList *
    getRaw() const
    { return raw.get(); }

    inline 
    operator WordList * () const
    { return getRaw(); }

    inline uint32_t
    length() const
    { return raw->length; }

    inline bool
    operator < (const WordListWrapper &b) const
    { return std::memcmp(raw->content, b.raw->content, std::min(length(), b.length())) < 0; }

    inline bool
    operator <= (const WordListWrapper &b) const
    { return std::memcmp(raw->content, b.raw->content, std::min(length(), b.length())) <= 0; }

    inline bool
    operator > (const WordListWrapper &b) const
    { return std::memcmp(raw->content, b.raw->content, std::min(length(), b.length())) > 0; }

    inline bool
    operator >= (const WordListWrapper &b) const
    { return std::memcmp(raw->content, b.raw->content, std::min(length(), b.length())) >= 0; }

    inline bool
    operator == (const WordListWrapper &b) const
    { return std::memcmp(raw->content, b.raw->content, std::min(length(), b.length())) == 0; }

    inline bool
    operator != (const WordListWrapper &b) const
    { return std::memcmp(raw->content, b.raw->content, std::min(length(), b.length())) != 0; }

    inline WordListIterator
    begin() const
    { return ::search::begin(*raw); }

    inline WordListIterator
    end() const
    { return ::search::end(*raw); }
};

}

#endif
