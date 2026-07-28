#include "cif.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <print>
#include <ranges>
#include <algorithm>
#include <unordered_map>

struct ATOM{
    float x,y,z;
    int s,m,i;
    std::string t,o,d;
};

void ciff::parse(const std::string& cif){
    std::ifstream file(cif);
    if(!file.is_open()) return;

    std::string line;
    int d = -1, s = -1, x = -1, y = -1, z = -1, m = -1, t = -1, i = -1, o = -1;
    int column = 0;
    int model = 1;
    
    while (std::getline(file, line)){
        if (line.starts_with("loop_")){
            continue;
        }

        if (line.starts_with("_atom_site.")){
            if (line.starts_with("_atom_site.group_PDB")) t = column;
            if (line.starts_with("_atom_site.id")) i = column;
            if (line.starts_with("_atom_site.label_atom_id")) d = column;
            if (line.starts_with("_atom_site.label_comp_id")) o = column;
            if (line.starts_with("_atom_site.label_seq_id")) s = column;
            if (line.starts_with("_atom_site.Cartn_x")) x = column;
            if (line.starts_with("_atom_site.Cartn_y")) y = column;
            if (line.starts_with("_atom_site.Cartn_z")) z = column;
            if (line.starts_with("_atom_site.pdbx_PDB_model_num")) m = column;
            column++;
        } if (line.starts_with("ATOM")){
            std::stringstream ss(line);
            std::string c;
            std::vector<std::string> r;
            // std::vector<std::string> v = {m};
            std::vector<ATOM> a;
            int nc = 0;
            while (ss >> c){
                nc++;
                r.push_back(c);
            }
                    
            int mc = std::max({d,s,x,y,z,m,t,i,o});
            if (r.size() > mc){
                        
                ATOM ca;
                ca.t = r[t];
                ca.i = std::stoi(r[i]);
                ca.d = r[d];
                ca.o = r[o];
                ca.s = std::stoi(r[s]);
                ca.x = std::stof(r[x]);
                ca.y = std::stof(r[y]);
                ca.z = std::stof(r[z]);
                ca.m = std::stoi(r[m]);

                a.push_back(ca);
            
                if (r.size() > m){
                    if (r[m] == "1"){
                        // std::println("{}, {}, {}, {}, {}, {}, {}, {}, {}", r[t], r[i], r[d], r[o], r[s], r[x], r[y], r[z], r[m]);
                    }
                }
                //std::println("{}", std::views::all(r));

            } else {
                std::println("no atoms? insert megamind meme");
                }
            
            // std::println("Found {},{},{},{},{}", x, y, z, m, r[m]);
        }

        if (line.starts_with("#")){
            if (x > -1 && y > -1 && z > -1){

            } else {
                x = -1;
                y = -1;
                z = -1;
                column = 0;
            }
        }
    }
}

// side note r = columns & c = rows... 
// yeah i didnt know what rows & columns were sybau