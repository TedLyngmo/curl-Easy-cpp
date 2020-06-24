#ifndef CURL_EASY_SLIST_HPP
#define CURL_EASY_SLIST_HPP

#if __cplusplus > 201703L
#    include <string_view>
#else
#    include <string>
#endif

namespace curl {

class SList {
    SList();
    SList(const SList&) = delete;
    SList(SList&&) noexcept;
    SList& operator=(const SList&) = delete;
    SList& operator=(SList&&) noexcept;
    ~SList();

#if __cplusplus >= 201703L
    void append(std::string_view str);
#else
    void append(const std::string& str);
#endif

public:
    curl_slist* list;
};

} // namespace curl

#endif
