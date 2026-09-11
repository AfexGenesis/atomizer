#pragma once
#include <string>
#include <vector>

struct atom {
    float x = 0.0f, y = 0.0f, z = 0.0f;
    int cx = 0, cy = 0, cz = 0;
    int i = 0;
    int s = 0;
    int m = 0;

    char t[8] = {0};
    char o[8] = {0};
    char d[8] = {0};
};

class ciff {
public: 
    std::vector<atom> parse(const std::string& cif);
};