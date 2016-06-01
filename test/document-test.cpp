#include <gtest/gtest.h>

#include "../src/document.hpp"

using namespace search;

static char TEST_DIR[] = "./.test.document.db";

static void clearIndexDB()
{
    std::system((std::string("rm -rf ") + TEST_DIR).c_str());
    std::system((std::string("mkdir ") + TEST_DIR).c_str());
}

TEST(document_test, add_get_erase_test)
{
    clearIndexDB();

    Document uut(TEST_DIR);
    std::string doc_attr("test.doc");
    count_t word_count = 10;

    auto id = uut.addDocument(word_count, doc_attr);
    ASSERT_EQ(1u, id);

    auto retrived = uut.getDocument(id);
    const Document::DocumentStruct *doc = reinterpret_cast<const Document::DocumentStruct*>(retrived.data());
    ASSERT_EQ(doc_attr, doc->attr);
    ASSERT_EQ(word_count, doc->word_count);

    ASSERT_EQ(1u, uut.getCount());

    uut.eraseDocument(id);

    ASSERT_EQ(0u, uut.getCount());
}

TEST(document_test, add_multiple_doc_test)
{
    clearIndexDB();

    Document uut(TEST_DIR);
    ASSERT_EQ(1u, uut.addDocument(10, "lalala"));
    ASSERT_EQ(2u, uut.addDocument(10, "lalala"));
    ASSERT_EQ(3u, uut.addDocument(10, "lalala"));
    ASSERT_EQ(4u, uut.addDocument(10, "lalala"));
    ASSERT_EQ(5u, uut.addDocument(10, "lalala"));
    ASSERT_EQ(6u, uut.addDocument(10, "lalala"));
    ASSERT_EQ(7u, uut.addDocument(10, "lalala"));

    ASSERT_EQ(7u, uut.getCount());
}

TEST(document, add_many_doc_test)
{
    clearIndexDB();

    Document uut(TEST_DIR);

    for (size_t i = 1; i < 1000; ++i) {
        ASSERT_EQ(i, uut.addDocument(10, "lalala"));
    }

    ASSERT_EQ(999u, uut.getCount());
}
