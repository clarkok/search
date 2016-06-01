#include <gtest/gtest.h>

#include "../src/utils.hpp"

using namespace search;

TEST(utils_split_word_test, basic_test)
{
    auto result = Utils::splitWord("hello world!");
    ASSERT_EQ(2u, result.size());
    ASSERT_EQ("hello", result[0]);
    ASSERT_EQ("world", result[1]);
}

TEST(utils_split_word_test, multiple_space_test)
{
    auto result = Utils::splitWord("a multiple      space sentence");
    ASSERT_EQ(4u, result.size());
    ASSERT_EQ("a", result[0]);
    ASSERT_EQ("multiple", result[1]);
    ASSERT_EQ("space", result[2]);
    ASSERT_EQ("sentence", result[3]);
}

TEST(utils_split_word_test, minus_test)
{
    auto result = Utils::splitWord("this is a connected-word");
    ASSERT_EQ(5u, result.size());
    ASSERT_EQ("this", result[0]);
    ASSERT_EQ("is", result[1]);
    ASSERT_EQ("a", result[2]);
    ASSERT_EQ("connected", result[3]);
    ASSERT_EQ("word", result[4]);
}

TEST(utils_split_word_test, single_quota_test)
{
    auto result = Utils::splitWord("it's a word");
    ASSERT_EQ(3u, result.size());
    ASSERT_EQ("it's", result[0]);
    ASSERT_EQ("a", result[1]);
    ASSERT_EQ("word", result[2]);
}

TEST(utils_split_word_test, start_or_end_with_dot_test)
{
    auto result = Utils::splitWord("... lalala ...");
    ASSERT_EQ(1u, result.size());
    ASSERT_EQ("lalala", result[0]);
}

TEST(utils_split_word_test, no_word_test)
{
    auto result = Utils::splitWord("...   ...");
    ASSERT_EQ(0u, result.size());
}

TEST(utils_split_word_test, to_lower_test)
{
    auto result = Utils::splitWord("WHAT A NICE DAY");
    ASSERT_EQ(4u, result.size());
    ASSERT_EQ("what", result[0]);
    ASSERT_EQ("a", result[1]);
    ASSERT_EQ("nice", result[2]);
    ASSERT_EQ("day", result[3]);
}

TEST(utils_filter_stop_word_test, basic_test)
{
    auto result = Utils::splitWord("What a nice day");
    Utils::filterStopWord(result);

    ASSERT_EQ(2u, result.size());
    ASSERT_EQ("nice", result[0]);
    ASSERT_EQ("day", result[1]);
}

TEST(utils_sentence_iterator_test, basic_test)
{
    std::string sentences[] = {
        "Input iterators are iterators that can be used in sequential input "
            "operations, where each value pointed by the iterator is read only "
            "once and then the iterator is incremented.",
        "All forward, bidirectional and random-access iterators are also valid "
            "input iterators.",
        "There is not a single type of input iterator:",
        "Each container may define its own specific iterator type able to "
            "iterate through it and access its elements.",
        "But all input iterators support at least the following operations:",
        "Where X is an input iterator type, a and b are objects of this "
            "iterator type, and t and u are objects of the type pointed by the "
            "iterator type.",
        "Algorithms requiring input iterators should be single-pass input "
            "algorithms:",
        "algorithms pass through an iterator position once at most."
    };

    std::string document;
    for (auto &sentence : sentences) {
        document += sentence;
    }

    Utils::SentenceIterator uut(document);
    for (auto &sentence : sentences) {
        ASSERT_EQ(sentence, *uut++);
    }
}

TEST(utils_lemmatize_word_test, basic_test)
{
    auto result = Utils::splitWord("making a better day");
    Utils::lemmatizeWord(result);

    ASSERT_EQ(4u, result.size());
    ASSERT_EQ("make", result[0]);
    ASSERT_EQ("a", result[1]);
    ASSERT_EQ("well", result[2]);
    ASSERT_EQ("day", result[3]);
}

TEST(utils_edit_distence_test, basic_test)
{
    ASSERT_EQ(1u, Utils::editDistence("doge", "dog"));
    ASSERT_EQ(1u, Utils::editDistence("dog", "doge"));
    ASSERT_EQ(1u, Utils::editDistence("deg", "dog"));
    ASSERT_EQ(1u, Utils::editDistence("dog", "deg"));
}
