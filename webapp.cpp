#include <iostream>
#include <string>
#include <curl/curl.h>
#include "pugixml.hpp"

#include "httplib.h"
#include "json.hpp"  // JSON library
#include <iostream>

using json = nlohmann::json;

struct RSSItem {
  std::string title;
  std::string link;
  std::string description;
};

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

std::vector<RSSItem> parseRSSFeed(const std::string& xmlData) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_string(xmlData.c_str());

  if (!result) {
      std::cerr << "Failed to parse RSS XML: " << result.description() << "\n";
      return {};  // Return an empty vector
  }

  std::vector<RSSItem> rssItems;

  // Iterate and store data into the vector
  for (pugi::xml_node item : doc.child("rss").child("channel").children("item")) {
      RSSItem rssItem;
      rssItem.title = item.child("title").text().as_string();
      rssItem.link = item.child("link").text().as_string();
      rssItem.description = item.child("description").text().as_string();
      rssItems.push_back(rssItem);
  }

  return rssItems;
}

int main() {
    httplib::Server svr;

    svr.Get("/", [](const httplib::Request&, httplib::Response& res) {
      res.set_content(readFile("index.html"), "text/html");
    });

    // GET endpoint: Returns JSON data
    // Will be used soon
    // svr.Get("/api/data", [](const httplib::Request&, httplib::Response& res) {
    //     json response = {
    //         {"message", "Hello from C++"},
    //         {"status", "success"},
    //         {"timestamp", time(nullptr)}
    //     };
    //     res.set_content(response.dump(), "application/json");
    // });

    // POST endpoint: Receives JSON and responds
    svr.Post("/api/feed", [](const httplib::Request& req, httplib::Response& res) {
      try {
        json received = json::parse(req.body);
        std::string rssUrl = received["url"];

        std::string rssData = fetchRSSFeed(rssUrl);
        std::vector<RSSItem> rssItems = parseRSSFeed(rssData);

        if (!rssItems.empty()) {
            json responseJson = json::array();
            for (const auto& item : rssItems) {
                responseJson.push_back({
                    {"title", item.title},
                    {"link", item.link},
                    {"description", item.description}
                });
            }

            res.set_content(responseJson.dump(), "application/json");
        } else {
            json response = {{"message", "No RSS items found."}, {"status", "error"}};
            res.set_content(response.dump(), "application/json");
        }
      } catch (...) {
          res.status = 400;
          res.set_content(R"({"error": "Invalid JSON"})", "application/json");
      }
    });

    std::cout << "Server running at http://localhost:8080" << std::endl;
    svr.listen("0.0.0.0", 8080);  // Start server on port 8080

    return 0;
}
