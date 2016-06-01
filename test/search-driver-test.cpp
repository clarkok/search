#include <gtest/gtest.h>

#include "../src/search-driver.hpp"

using namespace search;

static char TEST_DIR[] = "./.test.db";

static void clearIndexDB()
{
    auto retval = std::system((std::string("rm -rf ") + TEST_DIR).c_str());
    retval = std::system((std::string("mkdir ") + TEST_DIR).c_str());
    (void)retval;
}

TEST(search_driver_test, add_query_test)
{
    clearIndexDB();

    SearchDriver uut(TEST_DIR);

    uut.addDocument("database api", "test-database-api");
    uut.addDocument("database mysql", "test-database-mysql");
    uut.addDocument("mysql leveldb", "test-mysql-leveldb");

    {
        auto result = uut.query(uut.queryTokenList("database api mysql"));
        /*
        for (auto &doc : result) {
            std::cout << doc.id << "\t" << doc.attr << "\t" << doc.score << std::endl;
        }
        */
        ASSERT_EQ(3u, result.size());
        ASSERT_EQ("test-database-api", result[0].attr);
        ASSERT_EQ("test-database-mysql", result[1].attr);
        ASSERT_EQ("test-mysql-leveldb", result[2].attr);
    }

    {
        auto result = uut.query(uut.queryTokenList("database api"));
        /*
        for (auto &doc : result) {
            std::cout << doc.id << "\t" << doc.attr << "\t" << doc.score << std::endl;
        }
        */
        ASSERT_EQ(2u, result.size());
        ASSERT_EQ("test-database-api", result[0].attr);
        ASSERT_EQ("test-database-mysql", result[1].attr);
    }
}

TEST(search_driver_test, long_content_test)
{
    clearIndexDB();

    SearchDriver uut(TEST_DIR);

    uut.addDocument(
            "COCOA REVIEW "
            "Showers continued throughout the week in "
            "the Bahia cocoa zone, alleviating the drought since early "
            "January and improving prospects for the coming temporao, "
            "although normal humidity levels have not been restored, "
            "Comissaria Smith said in its weekly review.",
            "test");
}
