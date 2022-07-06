#ifndef CURL_EASY_SLIST_HPP
#define CURL_EASY_SLIST_HPP

#include "curl/curl.h"

#include <cerrno>
#include <cstring>
#include <initializer_list>
#include <stdexcept>
#include <string>

/*
** curl::Easy++
*/

namespace curl {
// A simple wrapper class around curl_slist
class Slist {
public:
    Slist() = default;
    Slist(const Slist&) = delete;
    Slist(Slist&&) noexcept;
    Slist& operator=(const Slist&) = delete;
    Slist& operator=(Slist&&) noexcept;
    ~Slist() noexcept;

    template<class Iter>
    Slist(Iter first, Iter last) {
        for(; first != last; ++first) {
            if(not append(*first))
                throw std::runtime_error(std::string("Slist: ") +
                                         std::strerror(errno));
        }
    }
    template<class T>
    Slist(std::initializer_list<T> il) : Slist(il.begin(), il.end()) {}

    bool append(const char*);
    // string_views are tricky since we can't expect them to be null terminated and
    // they can also be substrings of other strings, so they need to be converted to
    // string first.
    bool append(const std::string&);

    template<class Iter>
    bool append(Iter first, Iter last) {
        for(; first != last; ++first) {
            if(not append(*first)) return false;
        }
        return true;
    }

    template<class T>
    bool append(std::initializer_list<T> il) {
        return append(il.begin(), il.end());
    }

    struct curl_slist* get() {
        return m_slist;
    }

private:
    struct curl_slist* m_slist{};
};

} // namespace curl

#endif // CURL_EASY_SLIST_HPP
