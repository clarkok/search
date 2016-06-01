#ifndef _SEARCH_UTILS_HPP_
#define _SEARCH_UTILS_HPP_ 

#include <vector>
#include <string>
#include <set>
#include <map>
#include <iterator>

namespace search {

class Utils
{
public:
    using TokenList = std::vector<std::string>;
    using TokenSet = std::set<std::string>;
    using TokenMap = std::map<std::string, std::string>;

    static TokenList splitWord(std::string sentence);
    static TokenList splitWordOriginal(std::string sentence);
    static void filterStopWord(TokenList &list);
    static void lemmatizeWord(TokenList &list);
    static std::string readFile(std::string path);
    static size_t editDistence(std::string a, std::string b);

    struct SentenceIterator : public std::iterator<std::input_iterator_tag, std::string>
    {
    private:
        inline void
        _findEnd() const
        {
            static std::set<char> STOP_CHAR = {'!', '(', ')', '.', ':', ';', '?'};
            if (end != current) { return; }
            while (end != document.end()) {
                if (STOP_CHAR.find(*end++) != STOP_CHAR.end()) {
                    break;
                }
            }
        }

    public:
        std::string document;
        std::string::iterator current;
        mutable std::string::iterator end;

        SentenceIterator(std::string _document = "")
            : document(_document), current(document.begin()), end(current)
        { }

        SentenceIterator(const SentenceIterator &it)
            : document(it.document),
              current(document.begin() + (it.current - it.document.begin())),
              end(document.begin() + (it.end - it.document.begin()))
        { }

        inline bool
        operator == (const SentenceIterator &it) const
        {
            return (document == it.document) &&
                (current - document.begin() == it.current - it.document.begin());
        }

        inline bool
        operator != (const SentenceIterator &it) const
        { return !this->operator==(it); }

        inline std::string
        operator * () const
        {
            if (current == document.end()) { return ""; }
            _findEnd();
            return std::string(current, end);
        }

        inline SentenceIterator &
        operator ++()
        {
            if (current == document.end()) { return *this; }
            _findEnd();
            current = end;
            return *this;
        }

        inline SentenceIterator
        operator ++(int)
        {
            auto ret = *this;
            ++*this;
            return ret;
        }
    };
};

}

#endif
