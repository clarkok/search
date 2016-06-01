#ifndef _SEARCH_OUTPUT_HPP_
#define _SEARCH_OUTPUT_HPP_

#include <functional>

#include "search-driver.hpp"

namespace search {

using Outputer = std::function<void(const SearchDriver::Result &)>;

Outputer getTxtOutputer(std::ostream &os, size_t limit);
Outputer getColorOutputer(Utils::TokenList token_list, size_t limit);

}

#endif
