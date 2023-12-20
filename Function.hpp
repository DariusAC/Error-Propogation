#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <cstdint>
#include <string>
#include <cstring>
#include <cstdlib>

#include "Operation.hpp"
#include "Variables.hpp"

class Function {
private:
    Operation * f; // Array of functions that make up a function
    bool * initialized; // Array of whether each operation is fully initalized
    uint16_t size;
    std::string required_variables; // What variables does a function require? E.g. x+y requires both x and y
public:
    static std::string operators;

    Function(std::string equation);
    ~Function() { delete f; delete initialized; }

    double Evaluate(Variables * x);
    double PartialDerivative(Variables * x, char c, double percision); // Evaluate the given partial dervative 'c'
    double PropogateError(Variables * x, Variables * uncertainty, Variables * percision); // Runs error propogation given uncertainties and desired precisions using differentials

    void InitializeOperation(uint16_t i, char type, double value, char variable);
    std::string GetRequiredVariables() { return required_variables ; }
};

#endif
