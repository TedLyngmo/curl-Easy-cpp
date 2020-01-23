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
