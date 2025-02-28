#include <iostream>
#include <string>
#include <curl/curl.h>
#include "pugixml.hpp"

#include "httplib.h"
#include "json.hpp"  // JSON library
#include <iostream>

using json = nlohmann::json;

// Function to read HTML file
std::string readFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) return "File not found";
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

// Function to handle the HTTP response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// Function to fetch RSS feed from a given URL
std::string fetchRSSFeed(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string rssData;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rssData);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL Error: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    return rssData;
}

// Function to parse and display RSS feed data
void parseRSSFeed(const std::string& xmlData) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xmlData.c_str());

    if (!result) {
        std::cerr << "Failed to parse RSS XML: " << result.description() << "\n";
        return;
    }

    // Iterate through <item> tags in the RSS feed
    for (pugi::xml_node item : doc.child("rss").child("channel").children("item")) {
        std::cout << "Title: " << item.child("title").text().as_string() << std::endl;
        std::cout << "Link: " << item.child("link").text().as_string() << std::endl;
        std::cout << "Description: " << item.child("description").text().as_string() << std::endl;
        std::cout << "---------------------------------\n";
    }
}

int main() {
    // std::string rssUrl = "https://rss.nytimes.com/services/xml/rss/nyt/HomePage.xml";  // Example RSS feed
    // std::string rssData = fetchRSSFeed(rssUrl);

    // if (!rssData.empty()) {
    //     parseRSSFeed(rssData);
    // } else {
    //     std::cerr << "Failed to fetch RSS feed.\n";
    // }

    httplib::Server svr;

    // Serve HTML file
    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
      res.set_content(readFile("index.html"), "text/html");
  });

    // GET endpoint: Returns JSON data
    svr.Get("/api/data", [](const httplib::Request&, httplib::Response& res) {
        json response = {
            {"message", "Hello from C++"},
            {"status", "success"},
            {"timestamp", time(nullptr)}
        };
        res.set_content(response.dump(), "application/json");
    });

    // POST endpoint: Receives JSON and responds
    svr.Post("/api/data", [](const httplib::Request& req, httplib::Response& res) {
        try {
            json received = json::parse(req.body);
            std::string name = received["name"];
            json response = {{"message", "Hello, " + name + "!"}, {"status", "success"}};
            res.set_content(response.dump(), "application/json");
        } catch (...) {
            res.status = 400;
            res.set_content(R"({"error": "Invalid JSON"})", "application/json");
        }
    });

    std::cout << "Server running at http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);  // Start server on port 8080

    return 0;
}
