#pragma once
#include <vector>
#include "cif.hpp"

class grid{
    public:
        void build(std::vector<atom> atoms, float size = 4.0f);
        /*
        {
            atomss = std::move(atoms);
            sizee = size;
        }
        */
        const std::vector<atom>& all() const {return a;}
        size_t count() const {return a.size();}
        float size() const {return sizee;}
        void clear();

    private:
        std::vector<atom> a;
        float sizee = 4.0f;
};