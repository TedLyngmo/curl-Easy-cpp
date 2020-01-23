#include "curleasy.hpp"
#include <stdexcept>
#include <utility>

namespace curl {
namespace detail {

#if __cplusplus >= 201402L
    using std::exchange;
#else
template<class T, class U = T>
T exchange(T& obj, U&& new_value)
{
    T old_value = std::move(obj);
    obj = std::forward<U>(new_value);
    return old_value;
}
#endif

}  // namespace detail
}  // namespace curl

namespace curl {
// default constructor
Easy::Easy() : handle(curl_easy_init()) {
    if(handle == nullptr) {
        throw std::runtime_error(
            "curl::Easy default construction: curl_easy_init failed");
    }

    // Set "this" as data pointer in callbacks to be able to make a call to the
    // correct Easy object. There are a lot more callback functions you
    // could add here if you need them.
    setopt(CURLOPT_WRITEDATA, this);
    setopt(CURLOPT_DEBUGDATA, this);
    setopt(CURLOPT_XFERINFODATA, this);

    // Setup of proxy/callback functions. There should be one for each function
    // above.
    setopt(CURLOPT_WRITEFUNCTION, write_callback);
    setopt(CURLOPT_DEBUGFUNCTION, debug_callback);
    setopt(CURLOPT_XFERINFOFUNCTION, progress_callback);

    // some default options, remove those you usually don't want
    setopt(CURLOPT_NOPROGRESS, 0L);      // turn on progress callbacks
    setopt(CURLOPT_FOLLOWLOCATION, 1L);  // redirects
    setopt(CURLOPT_HTTPPROXYTUNNEL, 1L); // corp. proxies etc.
    // setopt(CURLOPT_REDIR_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
    setopt(CURLOPT_VERBOSE, 1L); // we want it all
}
// copy constructor
Easy::Easy(const Easy& other) : handle(curl_easy_duphandle(other.handle)) {
    if(handle == nullptr) {
        throw std::runtime_error(
            "curl::Easy copy construction: curl_easy_duphandle failed");
    }
    // State information is not shared when using curl_easy_duphandle. Only the
    // options you've set (so you can create one CURL object, set its options and
    // then use as a template for other objects. The document and debug data are
    // therefor also not copied.
}
// move constructor
Easy::Easy(Easy&& other) noexcept : handle(detail::exchange(other.handle, nullptr)) {}
// copy assignment
Easy& Easy::operator=(const Easy& other) {
    CURL* tmp_handle = curl_easy_duphandle(other.handle);
    if(tmp_handle == nullptr) {
        throw std::runtime_error(
            "curl::Easy copy assignment: curl_easy_duphandle failed");
    }
    // dup succeeded, now destroy any handle we might have and copy the tmp
    curl_easy_cleanup(handle);
    handle = tmp_handle;
    return *this;
}
// move assignment
Easy& Easy::operator=(Easy&& other) noexcept {
    std::swap(handle, other.handle);
    return *this;
}
Easy::~Easy() {
    curl_easy_cleanup(handle);
}

// To be able to use an instance of Easy with C interfaces if you don't add
// a function to this class for it, this operator will help
Easy::operator CURL*() {
    return handle;
}

// perform by supplying url
#if __cplusplus >= 201703L
CURLcode Easy::perform_url(std::string_view url) {
    setopt(CURLOPT_URL, url.data());
    return perform();
}
#else
CURLcode Easy::perform_url(const std::string& url) {
    setopt(CURLOPT_URL, url.c_str());
    return perform();
}
#endif

// perform with a previously supplied url
// override this to make preparations before actually doing the work
CURLcode Easy::perform() {
    return curl_easy_perform(handle);
}

// callbacks from proxy functions, override to capture data etc.
size_t Easy::on_write(char* /*ptr*/, size_t total_size) {
    return total_size;
}
int Easy::on_debug(curl_infotype /*type*/, char* /*data*/, size_t /*size*/) {
    return 0; // must return 0
}
int Easy::on_progress(curl_off_t /*dltotal*/, curl_off_t /*dlnow*/,
                      curl_off_t /*ultotal*/, curl_off_t /*ulnow*/) {
    return 0;
}

// private:

// a private class to initialize and cleanup curl once
class Easy::GlobalInit {
public:
    GlobalInit() noexcept :
        initialized(curl_global_init(CURL_GLOBAL_DEFAULT)==0U)
    {}
    GlobalInit(const GlobalInit&) = delete;
    GlobalInit(GlobalInit&&) = delete;
    GlobalInit& operator=(const GlobalInit&) = delete;
    GlobalInit& operator=(GlobalInit&&) = delete;
    ~GlobalInit() { if(initialized) { curl_global_cleanup(); } }

    explicit operator bool () const { return initialized; }
private:
    bool initialized;
};

Easy::GlobalInit Easy::setup_and_teardown{};

// callback functions - has to be static to work with the C interface in curl
// use the data pointer (this) that we set in the constructor and cast it back
// to a Easy* and call the event handler in the correct object.
size_t Easy::write_callback(char* ptr, size_t size, size_t nmemb, void* userdata) {
    Easy* ecurly = static_cast<Easy*>(userdata);
    return ecurly->on_write(ptr, nmemb * size); // size==1 really
}
int Easy::debug_callback(CURL* /*handle*/, curl_infotype type, char* data,
                         size_t size, void* userptr) {
    Easy* ecurly = static_cast<Easy*>(userptr);
    return ecurly->on_debug(type, data, size);
}
int Easy::progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow,
                            curl_off_t ultotal, curl_off_t ulnow) {
    Easy* ecurly = static_cast<Easy*>(clientp);
    return ecurly->on_progress(dltotal, dlnow, ultotal, ulnow);
}
//-----------------------------------------------------------------------------

// A fully functional extension to curl::Easy
// collected data getters
std::string const& EasyCollector::document() const {
    return m_document;
}
std::string const& EasyCollector::debug() const {
    return m_debug;
}

CURLcode EasyCollector::perform() {
    m_document.clear();
    m_debug.clear();
    return Easy::perform();
}
size_t EasyCollector::on_write(char* ptr, size_t total_size) {
    // store document data
    m_document.insert(m_document.end(), ptr, ptr + total_size);
    return total_size;
}
int EasyCollector::on_debug(curl_infotype /*type*/, char* data, size_t size) {
    // store debug data
    m_debug.insert(m_debug.end(), data, data + size);
    return 0; // must return 0
}
int EasyCollector::on_progress(curl_off_t /*dltotal*/, curl_off_t /*dlnow*/,
                               curl_off_t /*ultotal*/, curl_off_t /*ulnow*/) {
    // progress bar goes here
    return 0;
}

} // namespace curl
