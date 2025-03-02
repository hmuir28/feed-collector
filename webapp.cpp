#include <iostream>
#include <string>
#include <curl/curl.h>
#include "pugixml.hpp"

#include "httplib.h"
#include "json.hpp"
#include "controller/feed.controller.hpp"
#include <iostream>

int main() {
    httplib::Server svr;

    svr.Get("/", handleGETIndexUI);

    svr.Post("/api/feed", handlePOSTFeed);

    std::cout << "Server running at http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
