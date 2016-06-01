#include <algorithm>
#include <cstring>

#include "dict.hpp"

using namespace search;

namespace search {

const char * DICT_MAP[][2] = {
#include "dict/lemmtazer.inc"
};

const char * NOUN_SET[] = {
#include "dict/noun.inc"
};

const char * VERB_SET[] = {
#include "dict/verb.inc"
};

const char * ADJ_SET[] = {
#include "dict/adj.inc"
};

}


std::string
search::findInDictMap(std::string word)
{
    const char *key[2] = {word.c_str(), nullptr};
    auto iter = std::lower_bound(
        std::begin(DICT_MAP),
        std::end(DICT_MAP),
        key,
        [] (const char * const *a, const char * const *b) -> bool {
            return std::strcmp(a[0], b[0]) < 0;
        }
    );

    if (
        iter == std::end(DICT_MAP) ||
        (*iter)[0] != word
    ) {
        return "";
    }
    return (*iter)[1];
}

bool
search::findInNounSet(std::string word)
{
    auto iter = std::lower_bound(
        std::begin(NOUN_SET),
        std::end(NOUN_SET),
        word.c_str(),
        [] (const char *a, const char *b) -> bool {
            return std::strcmp(a, b) < 0;
        }
    );
    return (iter != std::end(NOUN_SET) && word == *iter);
}

bool
search::findInVerbSet(std::string word)
{
    auto iter = std::lower_bound(
        std::begin(VERB_SET),
        std::end(VERB_SET),
        word.c_str(),
        [] (const char *a, const char *b) -> bool {
            return std::strcmp(a, b) < 0;
        }
    );
    return (iter != std::end(VERB_SET) && word == *iter);
}

bool
search::findInAdjSet(std::string word)
{
    auto iter = std::lower_bound(
        std::begin(ADJ_SET),
        std::end(ADJ_SET),
        word.c_str(),
        [] (const char *a, const char *b) -> bool {
            return std::strcmp(a, b) < 0;
        }
    );
    return (iter != std::end(ADJ_SET) && word == *iter);
}
