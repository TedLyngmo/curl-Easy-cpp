# curl::Easy++

curl::Easy++ is set of C++ classes for [libcurl](https://github.com/curl/curl). The foundation class, `curl::Easy`, can be used as a base class for your own implementations and also serves as a base class for the other classes in curl::Easy++, such as `curl::EasyCollector` which can be used to fetch a document into a `std::string` straight out of the box.

# Building
A `build` directory is already created so:
```sh
% cmake --build build
```
This will create `libcurleasy.a` and an application called `example`. When linking your own programs you need to link with both `curleasy` and `curl`.

You can then run the demo program that fetches a document from my server. If everything is working, you'll get a nice greeting.
```
% build/demo/example
```
# Installing
This will create the directories `include/curleasy` and `lib` under the path you supply with `--prefix`.
```
% cmake --install build --prefix /usr/local
```


# Interface - in `namespace curl`
```cpp
class Easy {
public:
    Easy();
    Easy(const Easy& other);
    Easy(Easy&& other) noexcept;
    Easy& operator=(const Easy& other);
    Easy& operator=(Easy&& other) noexcept;
    virtual ~Easy();

    // To be able to use an instance of curl::Easy with C interfaces not
    // covered by curl::Easy++, this conversion operator will help
    inline explicit operator CURL*();

    // A generic curl_easy_setopt wrapper
    template<typename T>
    inline CURLcode setopt(CURLoption option, T v);

    inline CURLcode set_slist_opt(CURLoption option, Slist& slist);

    CURLcode perform_url(const char* url);
    CURLcode perform_url(const std::string& url);

    // perform with a previously supplied url (via setopt or perform_url)
    // override this to make preparations before actually doing the work
    virtual CURLcode perform();

    // event handlers - override these to capture data etc.
    virtual size_t on_write(char* ptr, size_t total_size);
    virtual int on_debug(curl_infotype type, char* data, size_t size);
    virtual int on_progress(curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
};
```
```cpp
class Slist {
public:
    Slist() = default;
    Slist(const Slist&) = delete;
    Slist(Slist&&) noexcept;
    Slist& operator=(const Slist&) = delete;
    Slist& operator=(Slist&&) noexcept;
    ~Slist() noexcept;

    template<class Iter>
    Slist(Iter first, Iter last);

    template<class T>
    Slist(std::initializer_list<T> il);

    bool append(const char* s);
    // string_views are tricky since we can't expect them to be null terminated and
    // they can also be substrings of other strings, so they need to be converted to
    // string first.
    bool append(const std::string&);

    template<class Iter>
    bool append(Iter first, Iter last);

    template<class T>
    bool append(std::initializer_list<T> il);

    struct curl_slist* get();
};
```

# Example
```cpp
#include "curleasy/curleasy.hpp"

#include <iostream>

int main() {
    constexpr auto width = 80;
    curl::EasyCollector ecurl; // our curl object

    CURLcode res = ecurl.perform_url("https://lyncon.se/curl-Easy-cpp.txt");

    std::cout
        << std::string(width, '-') << "\n"
        << "Result       : " << res << "\n"
        << "Document size: " << ecurl.document().size() << "\n"
        << "Debug size   : " << ecurl.debug().size() << "\n";

    std::cout
        << std::string(width, '-') << "\n"
        << ecurl.document()                 // print the captured document
        << std::string(width, '-') << "\n";
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
