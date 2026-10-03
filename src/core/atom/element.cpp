#include <algorithm>
#include <cctype>
#include <string>
#include "core/atom/element.hpp"

elementstylied style(const atom &a){
    std::string symbol(a.element);
    std::transform(symbol.begin(), symbol.end(), symbol.begin(), [](unsigned char c){ return std::toupper(c); });
    if (symbol == "H" || symbol == "D") return {0.31f, 1.20f, {0.95f, 0.95f, 0.95f, 1}};
    if (symbol == "C") return {0.76f, 1.70f, {0.62f, 0.66f, 0.70f, 1}};
    if (symbol == "N") return {0.71f, 1.55f, {0.20f, 0.36f, 0.90f, 1}};
    if (symbol == "O") return {0.66f, 1.52f, {0.90f, 0.20f, 0.22f, 1}};
    if (symbol == "S") return {1.05f, 1.80f, {0.96f, 0.82f, 0.18f, 1}};
    if (symbol == "P") return {1.07f, 1.80f, {0.96f, 0.52f, 0.16f, 1}};
    if (symbol == "F") return {0.57f, 1.47f, {0.32f, 0.76f, 0.30f, 1}};
    if (symbol == "CL") return {1.02f, 1.75f, {0.26f, 0.72f, 0.26f, 1}};
    if (symbol == "BR") return {1.20f, 1.85f, {0.68f, 0.27f, 0.19f, 1}};
    if (symbol == "I") return {1.39f, 1.98f, {0.57f, 0.22f, 0.68f, 1}};
    if (symbol == "FE") return {1.24f, 1.80f, {0.80f, 0.43f, 0.19f, 1}};
    if (symbol == "ZN") return {1.22f, 1.39f, {0.57f, 0.60f, 0.67f, 1}};
    if (symbol == "MG") return {1.41f, 1.73f, {0.28f, 0.72f, 0.37f, 1}};
    if (symbol == "CA") return {1.76f, 2.31f, {0.35f, 0.76f, 0.38f, 1}};
    return {0.77f, 1.70f, {0.70f, 0.42f, 0.72f, 1}};
}