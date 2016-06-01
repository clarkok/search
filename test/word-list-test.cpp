#include <gtest/gtest.h>

#include "../src/word-list.hpp"

using namespace search;

TEST(word_list_test, constructor_test)
{
    auto uut = WordList::fromStringVector({"word1", "word2", "word3"});

    std::vector<std::string> result;
    for (auto word : *uut) {
        result.push_back(word);
    }

    ASSERT_EQ("word1", result[0]);
    ASSERT_EQ("word2", result[1]);
    ASSERT_EQ("word3", result[2]);

    WordList::free(uut);
}
