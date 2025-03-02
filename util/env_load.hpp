#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

void loadEnvVariables(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        // Skip empty lines or comment lines
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Find the position of '=' sign to separate key and value
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            // Set the environment variable
            setenv(key.c_str(), value.c_str(), 1); // Overwrite if exists
        }
    }

    file.close();
}

