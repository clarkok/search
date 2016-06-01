#include <string>
#include <limits>
#include <set>

#include "rlutil/rlutil.h"

#include "output.hpp"
#include "utils.hpp"

using namespace search;

Outputer
search::getTxtOutputer(std::ostream &os, size_t limit)
{
    return [&os, limit] (const SearchDriver::Result &results) {
        std::vector<size_t> field_size({8, 20, 10});
        auto counter = limit ? limit : std::numeric_limits<size_t>::max();

        for (auto &result : results) {
            if (!counter--) break;
            if (result.attr.length() + 3 > field_size[1]) {
                field_size[1] = result.attr.length() + 3;
            }
        }

        std::string header = " id";
        while (header.length() < field_size[0]) {
            header.push_back(' ');
        }
        header += " | path";
        while (header.length() < field_size[0] + field_size[1]) {
            header.push_back(' ');
        }
        header += " | score";
        std::cout << header << std::endl;

        counter = limit ? limit : std::numeric_limits<size_t>::max();
        for (auto &result : results) {
            if (!counter--) break;
            std::string line(" " + std::to_string(result.id));
            while (line.length() < field_size[0]) {
                line.push_back(' ');
            }
            line += " | ";

            line += result.attr;
            while (line.length() < field_size[0] + field_size[1]) {
                line.push_back(' ');
            }
            line += " | ";

            line += std::to_string(result.score);
            os << line << std::endl;
        }
    };
}

Outputer
search::getColorOutputer(Utils::TokenList token_list, size_t limit)
{
    return [token_list, limit] (const SearchDriver::Result &results) {
        std::set<std::string> searched_word;
        for (auto &token : token_list) {
            searched_word.emplace(token);
        }

        auto counter = limit ? limit : std::numeric_limits<size_t>::max();
        for (auto &result : results) {
            if (!counter--) break;

            rlutil::saveDefaultColor();
            rlutil::setColor(rlutil::WHITE);
            std::cout << result.attr << std::endl;
            rlutil::resetColor();

            auto content = Utils::readFile(result.attr);
            Utils::SentenceIterator sentences(content);
            for (
                auto sentence = *sentences++;
                sentence.length();
                sentence = *sentences++
            ) {
                auto original = Utils::splitWordOriginal(sentence);
                auto token_list = Utils::splitWord(sentence);
                Utils::lemmatizeWord(token_list);
                bool output = false;
                size_t j = 0;
                for (size_t i = 0; i < token_list.size(); ++i) {
                    if (searched_word.find(token_list[i]) != searched_word.end()) {
                        output = true;
                        while (j < i) {
                            std::cout << original[j++] << " ";
                        }
                        rlutil::saveDefaultColor();
                        rlutil::setColor(rlutil::RED);
                        std::cout << original[j++] << " ";
                        rlutil::resetColor();
                    }
                }

                if (output) {
                    while (j < token_list.size()) {
                        std::cout << original[j++] << " ";
                    }
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
        }
    };
}
