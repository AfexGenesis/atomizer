
#include "atom.hpp"

atomized::atomized(){
    vertices = {
        {{-1.0f, -1.0f}},
        {{ 1.0f, -1.0f}},
        {{ 1.0f,  1.0f}},
        {{-1.0f,  1.0f}}
    };
    indices = {0, 1, 2, 0, 2, 3};
}