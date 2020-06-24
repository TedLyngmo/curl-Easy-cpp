#include "slist.hpp"

namespace curl {

SList::SList() : list{nullptr} {}
SList::SList(SList&&) noexcept

{
}
SList& SList::operator=(SList&&) noexcept {
    return *this;
}
SList::~SList() {
}

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
