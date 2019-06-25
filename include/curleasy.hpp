#include "curl/curl.h"
#include <string_view>

/*
** curl::Easy++
*/

namespace curl {
// A simple wrapper class for the easy_curl functions in libcurl
class Easy {
public:
    Easy();
    Easy(const Easy& other);
    Easy(Easy&& other);
    Easy& operator=(const Easy& other);
    Easy& operator=(Easy&& other);
    virtual ~Easy();

    // To be able to use an instance of curl::Easy with C interfaces not
    // covered by curl::Easy++, this conversion operator will help
    inline operator CURL*() { return handle; }

    // A generic curl_easy_setopt wrapper
    template<typename T>
    inline CURLcode setopt(CURLoption option, T v) {
        return curl_easy_setopt(handle, option, v);
    }

    // perform by supplying url
    CURLcode perform_url(std::string_view url);

    // perform with a previously supplied url (via setopt or perform_url)
    // override this to make preparations before actually doing the work
    virtual CURLcode perform();

    // event handlers - override these to capture data etc.
    virtual size_t on_write(char* ptr, size_t total_size);
    virtual int on_debug(curl_infotype type, char* data, size_t size);
    virtual int on_progress(curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal,
                            curl_off_t ulnow);

private:
    // a private class to initialize and cleanup curl once
    class GlobalInit;

    // callback functions - has to be static to work with the C interface in curl
    // use the data pointer (this) that we set in the constructor and cast it back
    // to a Easy* and call the event handler in the correct object.
    static size_t write_callback(char* ptr, size_t size, size_t nmemb,
                                 void* userdata);
    static int debug_callback(CURL* handle, curl_infotype type, char* data,
                              size_t size, void* userptr);
    static int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow,
                                 curl_off_t ultotal, curl_off_t ulnow);

    // resources
    CURL* handle;

    // a static initializer object
    static GlobalInit setup_and_teardown;
};

// A fully functional extension to curl::Easy
class EasyCollector : public curl::Easy {
public:
    // collected data getters
    std::string const& document() const { return m_document; }
    std::string const& debug() const { return m_debug; }

    CURLcode perform() override {
        m_document.clear();
        m_debug.clear();
        return Easy::perform();
    }
    size_t on_write(char* ptr, size_t total_size) override {
        // store document data
        m_document.insert(m_document.end(), ptr, ptr + total_size);
        return total_size;
    }
    int on_debug(curl_infotype /*type*/, char* data, size_t size) override {
        // store debug data
        m_debug.insert(m_debug.end(), data, data + size);
        return 0; // must return 0
    }
    int on_progress(curl_off_t /*dltotal*/, curl_off_t /*dlnow*/,
                    curl_off_t /*ultotal*/, curl_off_t /*ulnow*/) override {
        // progress bar goes here
        return 0;
    }

private:
    std::string m_document{};
    std::string m_debug{};
};

} // namespace curl
