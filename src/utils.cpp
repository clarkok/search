#include <cctype>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "utils.hpp"
#include "dict.hpp"

using namespace search;

Utils::TokenList
Utils::splitWord(std::string sentence)
{
    TokenList ret;
    ret.emplace_back("");

    for (auto ch : sentence) {
        if (std::isalnum(ch)) {
            ret.back().push_back(std::tolower(ch));
        }
        else {
            if (ret.back().length()) {
                ret.emplace_back("");
            }
        }
    }

    if (!ret.back().length()) {
        ret.pop_back();
    }
    return ret;
}

Utils::TokenList
Utils::splitWordOriginal(std::string sentence)
{
    TokenList ret;
    ret.emplace_back("");

    for (auto ch : sentence) {
        if (std::isalnum(ch)) {
            ret.back().push_back(ch);
        }
        else {
            if (ch != ' ' && ch != '\n' && ret.back().length()) {
                ret.back().push_back(ch);
            }
            if (ret.back().length()) {
                ret.emplace_back("");
            }
        }
    }

    if (!ret.back().length()) {
        ret.pop_back();
    }
    return ret;
}

void
Utils::filterStopWord(TokenList &list)
{
    static const TokenSet STOP_WORDS = {
#include "dict/stop-words.inc"
    };

    list.erase(
            std::remove_if(
                list.begin(),
                list.end(),
                [&](const std::string &word) {
                    return word.length() == 1 || STOP_WORDS.find(word) != STOP_WORDS.end();
                }
            ),
            list.end()
    );
}

static inline std::string
rewriteTail(std::string word, std::string pattern, std::string replace)
{
    if (word.length() <= pattern.length()) {
        return word;
    }

    if (
        word.compare(
            word.length() - pattern.length(),
            std::string::npos,
            pattern
        ) == 0
    ) {
        return word.erase(word.length() - pattern.length()) + replace;
    }

    return word;
}

void
Utils::lemmatizeWord(TokenList &list)
{
    for (auto &word : list) {
        auto dict_result = findInDictMap(word);
        if (dict_result.length()) {
            word = dict_result;
            continue;
        }

        if (findInNounSet(word) || findInVerbSet(word) || findInAdjSet(word)) {
            continue;
        }

        std::string replaced;

#define try_rewrite(findInSet, pattern, replace)            \
        replaced = rewriteTail(word, pattern, replace);     \
        if (replaced != word) {                             \
            if (findInSet(replaced)) {                      \
                word = replaced;                            \
                continue;                                   \
            }                                               \
        }

        try_rewrite(findInNounSet,  "s",    "");
        try_rewrite(findInNounSet,  "ses",  "s");
        try_rewrite(findInNounSet,  "ves",  "f");
        try_rewrite(findInNounSet,  "xes",  "x");
        try_rewrite(findInNounSet,  "zes",  "z");
        try_rewrite(findInNounSet,  "ches", "ch");
        try_rewrite(findInNounSet,  "shes", "sh");
        try_rewrite(findInNounSet,  "men",  "man");
        try_rewrite(findInNounSet,  "ies",  "y");

        try_rewrite(findInVerbSet,  "s",    "");
        try_rewrite(findInVerbSet,  "ies",  "y");
        try_rewrite(findInVerbSet,  "es",   "");
        try_rewrite(findInVerbSet,  "es",   "e");
        try_rewrite(findInVerbSet,  "ed",   "");
        try_rewrite(findInVerbSet,  "ed",   "e");
        try_rewrite(findInVerbSet,  "ing",  "");
        try_rewrite(findInVerbSet,  "ing",  "e");

        try_rewrite(findInAdjSet,   "er",   "");
        try_rewrite(findInAdjSet,   "er",   "e");
        try_rewrite(findInAdjSet,   "est",  "");
        try_rewrite(findInAdjSet,   "est",  "e");

#undef try_rewrite
    }
}

std::string
Utils::readFile(std::string path)
{
    std::ifstream fs(path);
    return std::string(std::istreambuf_iterator<char>(fs),
                       std::istreambuf_iterator<char>());
}

size_t
Utils::editDistence(std::string a, std::string b)
{
    if (a.length() < b.length()) {
        std::swap(a, b);
    }

    std::vector<size_t> current(a.length() + 1, 0),
                        last(a.length() + 1, 0);

    for (size_t i = 0; i <= a.length(); ++i) {
        last[i] = i;
    }

    for (size_t j = 1; j <= b.length(); ++j) {
        current[0] = j;
        for (size_t i = 1; i <= a.length(); ++i) {
            current[i] = last[i - 1] + (a[i] != b[j] ? 1 : 0);
            if (current[i] > last[i] + 1) {
                current[i] = last[i] + 1;
            }
            if (current[i] > current[i - 1] + 1) {
                current[i] = current[i - 1] + 1;
            }
        }
        std::swap(current, last);
    }

    return last[a.length()];
}
