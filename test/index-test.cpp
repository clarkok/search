#include <gtest/gtest.h>
#include <cstdlib>

#include "../src/index.hpp"

using namespace search;

static char TEST_DIR[] = "./.test.index.db";

static void clearIndexDB()
{
    std::system((std::string("rm -rf ") + TEST_DIR).c_str());
    std::system((std::string("mkdir ") + TEST_DIR).c_str());
}

TEST(index_test, add_get_test)
{
    clearIndexDB();

    Index uut(TEST_DIR);

    auto key = WordList::fromStringVector({"hello"});
    docid_t id = 1;
    count_t count = 10;
    uut.addIndex(key, id, count);

    auto value = uut.getIndex(key);
    const Index::IndexRow *row = reinterpret_cast<const Index::IndexRow*>(value.data());
    ASSERT_EQ(sizeof(Index::IndexItem) + sizeof(count_t), value.length());

    ASSERT_EQ(id, row->items[0].id);
    ASSERT_EQ(count, row->items[0].count);
}
