#ifndef _SEARCH_SEARCH_HPP_
#define _SEARCH_SEARCH_HPP_

#include <cstdint>

namespace search {

#define VERSION         "v0.0.1"
#define DEFAULT_DB_DIR  ".search/"
#define ENV_VAR_NAME    "SEARCH_INDEX_DIR"

    typedef std::uint32_t docid_t;
    typedef std::uint32_t count_t;
    
    constexpr static int BESIDE_PAIR_WEIGHT = 8;
    constexpr static int BESIDE_NEXT_WEIGHT = 4;
    constexpr static double TYPO_LIMIT = 4;

    extern bool is_debug;
}

#endif
