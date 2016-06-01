#include "document.hpp"

using namespace search;

char Document::DOCUMENT_DIR[] = "document.db";
char Document::COUNT_KEY[] = {0, 0, 0, 0};

static_assert(sizeof(Document::COUNT_KEY) == sizeof(count_t), "these two should kept same");
