#ifndef _SEARCH_DICT_HPP_
#define _SEARCH_DICT_HPP_

#include <string>

namespace search {

std::string findInDictMap(std::string word);
bool findInNounSet(std::string word);
bool findInVerbSet(std::string word);
bool findInAdjSet(std::string word);

}

#endif
