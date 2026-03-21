#ifndef ENV_H_INCLUDED
#define ENV_H_INCLUDED

#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <cstdlib>

using namespace std;

void load_env_file(const string& filepath = ".env") {
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "Could not open .env file\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        auto delimiter = line.find('=');
        if (delimiter == string::npos) continue;

        string key   = line.substr(0, delimiter);
        string value = line.substr(delimiter + 1);

        if (value.size() >= 2 &&
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }

        _putenv_s(key.c_str(), value.c_str());  // Windows version
    }
}

#endif // ENV_H_INCLUDED
