#ifndef _SEARCH_SEARCH_DRIVER_HPP_
#define _SEARCH_SEARCH_DRIVER_HPP_

#include <memory>
#include <vector>
#include <list>

#include "utils.hpp"
#include "search.hpp"
#include "index.hpp" 
#include "document.hpp"

namespace search {

class SearchDriver
{
    std::unique_ptr<Index> index;
    std::unique_ptr<Document> document;

    struct QueryDocument
    {
        docid_t id;
        count_t word_count;
        std::vector<double> count;

        QueryDocument(docid_t id, count_t word_count)
            : id(id), word_count(word_count)
        { }
    };

    using DocVector = std::list<QueryDocument>;

    DocVector queryIndex(WordList *list, DocVector &result, int result_index);

public:
    struct FoundDocument
    {
        docid_t id;
        std::string attr;
        double score;

        FoundDocument(docid_t id, std::string attr, double score)
            : id(id), attr(attr), score(score)
        { }
    };

    using Result = std::vector<FoundDocument>;

    SearchDriver(std::string path);
    ~SearchDriver() = default;

    docid_t addDocument(std::string content, std::string attr);
    Result query(const Utils::TokenList &token_list);
    Utils::TokenList queryTokenList(std::string query);

    void dumpIndex(std::ostream &os)
    { index->dumpIndex(os); }
};

}

#endif
