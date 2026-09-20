#pragma once
#include <string>
#include <vector>

struct atom {
    float x = 0.0f, y = 0.0f, z = 0.0f;
    float occupancy = 1.0f;
    int cx = 0, cy = 0, cz = 0;
    int i = 0;
    int s = 0;
    int m = 0;
    int charge = 0;

    char t[8] = {0};
    char o[8] = {0};
    char d[8] = {0};
    char element[4] = {0};
    char chain[16] = {0};
    char alt[4] = {0};
};

class ciff {
public: 
    std::vector<atom> parse(const std::string& cif);
};