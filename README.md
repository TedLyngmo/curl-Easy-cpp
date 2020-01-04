# curl::Easy++

curl::Easy++ is set of C++ classes for [libcurl](https://github.com/curl/curl). The foundation class, `curl::Easy`, can be used as a base class for your own implementations and also serves as a base class for the other classes in curl::Easy++, such as `curl::EasyCollector` which can be used to fetch a document into a `std::string` straight out of the box.

# Building
```sh
% make
```
This will create `libcurleasy.a` and an application called `example`. When linking your own programs you need to link with both `-lcurl` and `-lcurleasy`.

# Interface
```cpp
namespace curl {
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

    // set options supported by curl_easy_setopt()
    CURLcode setopt(CURLoption option, parameter);

    // perform by supplying a url
    CURLcode perform_url(std::string_view url);

    // perform with a previously supplied url (via setopt or perform_url)
    // override this to make preparations before actually doing the work
    virtual CURLcode perform();

    // event handlers - override these to capture data etc.
    virtual size_t on_write(char* ptr, size_t total_size);
    virtual int on_debug(curl_infotype type, char* data, size_t size);
    virtual int on_progress(curl_off_t dltotal, curl_off_t dlnow,
                            curl_off_t ultotal, curl_off_t ulnow);
};
}
```

# Example
```cpp
#include "curleasy/curleasy.hpp"
#include <iostream>

int main() {
    curl::EasyCollector ecurl; // there's no need to initialize libcurl

    CURLcode res = ecurl.perform_url("http://www.google.com/");
    std::cout << "result: " << res << "\n";

    std::cout << "--- document size " << ecurl.document().size() << " captured ---\n"
              << ecurl.document() << "---------\n\n";
    std::cout << "--- debug size " << ecurl.debug().size() << " captured ---\n"
              << ecurl.debug() << "---------\n";
}
```
# Extending
Extending curl::Easy is supposed to be very easy. The full curl::EasyCollector class implementation used in the example above looks like this:
```cpp
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
```
