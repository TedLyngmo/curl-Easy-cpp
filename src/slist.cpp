#include "curleasy/slist.hpp"

#include <utility>

namespace curl {

Slist::Slist(Slist&& rhs) noexcept : m_slist(std::exchange(rhs.m_slist, nullptr)) {}

Slist& Slist::operator=(Slist&& rhs) noexcept {
    std::swap(m_slist, rhs.m_slist);
    return *this;
}

Slist::~Slist() {
    if(m_slist) curl_slist_free_all(m_slist);
}

bool Slist::append(const char* str) {
    auto nl = curl_slist_append(m_slist, str);
    if(nl == nullptr) return false;
    m_slist = nl;
    return true;
}

bool Slist::append(const std::string& str) {
    return append(str.c_str());
}

} // namespace curl
