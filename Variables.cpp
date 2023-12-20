#include "Variables.hpp"

Variables::Variables() {
    keys = "";
    values = (double *)malloc(52*sizeof(double));
}
Variables::~Variables() {
    // delete values; <- This seems deleted anyway??? 
}
// Creates a map between a single charecter c, upper or lowercase, and a double v
void Variables::SetVar(char key, double v) {
    uint8_t index;
    if (key >= 'a' && key <= 'z') // i.e. lowercase
        index = key - 'a';
    else if (key >= 'A' && key <= 'Z') // i.e. uppercase
        index = key - 'A' + 26;
    else {
        std::cerr << "Invalid variable name " << key << std::endl;
        return;
    }

    keys += key;
    values[index] = v;
}
// Retrieves the value associated with the key c
double Variables::GetVar(char key) {
    if (!keys.contains(key)) {
        std::cerr << "No such variable found " << key << std::endl;
        return 0.0;
    }
    uint8_t index;
    if (key >= 'a' && key <= 'z') // i.e. lowercase
        index = key - 'a';
    else if (key >= 'A' && key <= 'Z') // i.e. uppercase
        index = key - 'A' + 26;
    else {
        std::cerr << "Invalid variable name " << key << std::endl;
        return 0.0;
    }
    return values[index];
}
