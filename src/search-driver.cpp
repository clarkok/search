#include <map>
#include <cmath>
#include <algorithm>

#include "search-driver.hpp"
#include "utils.hpp"

using namespace search;

SearchDriver::SearchDriver(std::string path)
    : index(new Index(path)), document(new Document(path))
{ }

docid_t
SearchDriver::addDocument(std::string content, std::string attr)
{
    std::map<WordListWrapper, count_t> counter;

    Utils::SentenceIterator sentences(content);
    count_t word_count = 0;

    for (
        auto sentence = *sentences++;
        sentence.length();
        sentence = *sentences++
    ) {
        auto token_list = queryTokenList(sentence);

        word_count += token_list.size();

        for (
            auto word_iter = token_list.begin();
            word_iter != token_list.end();
            word_iter++
        ) {
            counter[WordListWrapper(std::vector<std::string>{*word_iter})]++;

            if (word_iter >= token_list.begin() + 1) {
                counter[WordListWrapper({
                    *(word_iter - 1),
                    *word_iter
                })] += BESIDE_PAIR_WEIGHT;
            }

            if (word_iter >= token_list.begin() + 2) {
                counter[WordListWrapper({
                    *(word_iter - 2),
                    *word_iter
                })] += BESIDE_NEXT_WEIGHT;
            }
        }
    }

    auto ret = document->addDocument(word_count, attr);
    for (auto &counter_pair : counter) {
        index->addIndex(counter_pair.first, ret, counter_pair.second);
    }

    return ret;
}

SearchDriver::Result
SearchDriver::query(const Utils::TokenList &token_list)
{
    DocVector results;
    int result_index = 0;

    auto total_count = document->getCount() * 1.0;
    QueryDocument query_doc(-1, token_list.size());
    std::map<WordListWrapper, count_t> query_word_count;

    for (
        auto word_iter = token_list.begin();
        word_iter != token_list.end();
        word_iter++
    ) {
        query_word_count[WordListWrapper(std::vector<std::string>{*word_iter})]++;
        if (word_iter >= token_list.begin() + 1) {
            query_word_count[WordListWrapper({
                *(word_iter - 1),
                *word_iter
            })]++;
        }
        if (word_iter >= token_list.begin() + 2) {
            query_word_count[WordListWrapper({
                *(word_iter - 2),
                *word_iter
            })]++;
        }
    }

    for (auto &query_word_pair : query_word_count) {
        queryIndex(query_word_pair.first, results, result_index++);
        query_doc.count.emplace_back(
            (query_word_pair.second * 1.0 / query_doc.word_count) * total_count
        );
    }

    std::vector<FoundDocument> ret;
    for (auto &result : results) {
        ret.emplace_back(result.id, document->getDocumentAttr(result.id), 0);

        double w_d_q = 0;
        double w_d_d = 0;
        double w_q_q = 0;
        for (auto i = 0; i < result_index; ++i) {
            w_d_q += result.count[i] * query_doc.count[i];
            w_d_d += result.count[i] * result.count[i];
            w_q_q += query_doc.count[i] * query_doc.count[i];
        }

        ret.back().score = w_d_q / (std::sqrt(w_d_d) * std::sqrt(w_q_q));
    }

    std::sort(ret.begin(), ret.end(), [](const FoundDocument &a, const FoundDocument &b) { return a.score > b.score; });
    return ret;
}

SearchDriver::DocVector
SearchDriver::queryIndex(WordList *list, DocVector &result, int result_index)
{
    auto indexes = index->getIndex(list);
    const Index::IndexRow *row = reinterpret_cast<const Index::IndexRow*>(indexes.data());

    auto global_freq = std::log(document->getCount() * 1.0 / row->total_count + 1);
    auto doc_iter = result.begin();
    auto row_size = Index::rowSize(indexes);

    if (indexes.length()) {
        for (auto i = 0u; i < row_size; ++i) {
            while (doc_iter != result.end() && doc_iter->id < row->items[i].id) {
                doc_iter->count.emplace_back(0);
                ++doc_iter;
            }

            if (doc_iter != result.end() && doc_iter->id == row->items[i].id) {
                doc_iter->count.emplace_back(
                    (row->items[i].count * 1.0 / doc_iter->word_count) * global_freq
                );
                ++doc_iter;
            }
            else {
                auto new_iter = result.emplace(doc_iter, row->items[i].id, document->getDocumentWordCount(row->items[i].id));
                for (auto i = 0; i < result_index; ++i) {
                    new_iter->count.emplace_back(0);
                }
                new_iter->count.emplace_back(
                    (row->items[i].count * 1.0 / new_iter->word_count) * global_freq
                );
            }
        }
    }

    while (doc_iter != result.end()) {
        doc_iter->count.emplace_back(0);
        ++doc_iter;
    }

    return result;
}

Utils::TokenList
SearchDriver::queryTokenList(std::string query)
{
    auto token_list = Utils::splitWord(query);
    Utils::filterStopWord(token_list);
    Utils::lemmatizeWord(token_list);

    double freq_o = index->getIndexWordCount(
        WordListWrapper(std::vector<std::string>{token_list[0]})) + 1;
    auto it = index->getIterator(leveldb::Slice(token_list[0].c_str(), 1));
    std::string replacement = "";
    double max_score = 0;
    while (
        it->Valid() && 
        std::memcmp(
            it->key().data(),
            token_list[0].substr(0, 1).c_str(),
            1
        ) == 0
    ) {
        auto list = WordListWrapper(it->key());
        if (std::strlen(*list.begin()) != list.length() - 2) {
            // not a single word
            it->Next();
            continue;
        }

        std::string replace = *list.begin();
        if (replace == token_list[0]) {
            it->Next();
            continue;
        }
        double score = std::log(1 + *reinterpret_cast<const count_t*>(it->value().data()) / freq_o) *
                       (1.0 / (Utils::editDistence(token_list[0], replace)));

        if (score > max_score) {
            replacement = replace;
            max_score = score;
        }
        it->Next();
        if (is_debug) {
            std::cout << "replace `" << token_list[0] << "` candidate `" << replace << "` score " << score << std::endl;
        }
    }
    delete it;
    if (max_score > TYPO_LIMIT) {
        token_list[0] = replacement;
    }

    for (size_t i = 1; i < token_list.size(); ++i) {
        auto prev = token_list[i - 1];
        auto original = token_list[i];

        double freq_d_o = index->getIndexWordCount(WordListWrapper({prev, original})) + 1;
        double freq_o = index->getIndexWordCount(
            WordListWrapper(std::vector<std::string>{original})) + 1;
        auto it = index->getIterator(prev);
        std::string replacement = "";
        double max_score = 0;

        while (
            it->Valid() &&
            prev == *WordListWrapper(it->key()).begin()
        ) {
            auto list = WordListWrapper(it->key());
            if (std::strlen(*list.begin()) == list.length() - 2) {
                // a single word
                it->Next();
                continue;
            }

            auto word_it = list.begin();
            ++word_it;
            std::string replace = *word_it;
            if (replace == original) {
                it->Next();
                continue;
            }
            double score = 
                std::log(1 +
                        1.0 * (index->getIndexWordCount(
                                WordListWrapper(std::vector<std::string>{replace}))) /
                        freq_o
                    ) *
                std::log(1 +
                        1.0 * (*reinterpret_cast<const count_t *>(it->value().data())) /
                        freq_d_o
                    ) *
                (1.0 / Utils::editDistence(original, replace));
            if (score > max_score) {
                replacement = replace;
                max_score = score;
            }
            it->Next();
            if (is_debug) {
                std::cout << "replace `" << token_list[i] << "` candidate `" << replace << "` score " << score << std::endl;
            }
        }
        if (max_score > TYPO_LIMIT) {
            token_list[i] = replacement;
        }
        delete it;
    }

    return token_list;
}
