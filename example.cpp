#include "include/curleasy.hpp"
#include <iostream>

int main() {
    curl::EasyCollector ecurl; // our curl object

    CURLcode res = ecurl.perform_url("http://www.google.com/");
    std::cout << "result: " << res << "\n";

    std::cout << "--- document size " << ecurl.document().size() << " captured ---\n"
              << ecurl.document() << "---------\n\n";
    std::cout << "--- debug size " << ecurl.debug().size() << " captured ---\n"
              << ecurl.debug() << "---------\n";
}
