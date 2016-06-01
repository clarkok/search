#include <cstdlib>
#include <cstring>

#include "word-list.hpp"

using namespace search;

WordList *
WordList::fromStringVector(const std::vector<std::string> &words)
{
    uint32_t length = 0;
    for (auto &word : words) {
        length += word.length() + 1;
    }

    WordList *ret = reinterpret_cast<WordList*>(std::malloc(length + 1 + sizeof(WordList)));
    auto ptr = ret->content;
    for (auto &word : words) {
        std::memcpy(ptr, word.c_str(), word.length() + 1);
        ptr += word.length() + 1;
    }
    ret->content[length] = '\0';
    ret->length = length + 1;

    return ret;
}

WordList *
WordList::fromSlice(leveldb::Slice slice)
{
    WordList *ret = reinterpret_cast<WordList*>(std::malloc(slice.size() + sizeof(WordList)));
    ret->length = slice.size();
    std::memcpy(ret->content, slice.data(), slice.size());
    return ret;
}

void
WordList::free(WordList *list)
{ std::free(list); }
