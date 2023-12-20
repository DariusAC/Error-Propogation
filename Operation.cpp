#include "Operation.hpp"

// Evalute the given operation with the passed variables
double Operation::Evaluate(Variables * x) {

    // If its an equals, simply return the value

    if (type == '=') {
        if (var != 0)
            return x->GetVar(var);
        return value;
    }

    // Otherwise apply the operation

    double v1 = o1->Evaluate(x);
    double v2 = o2->Evaluate(x);

    switch(type) {
        case '+':
            return v1 + v2;
        case '-':
            return v1 - v2;
        case '*':
            return v1 * v2;
        case '/':
            return v1 / v2;
        case '^':
            return pow(v1, v2);
        default:
            std::cout << "Unknown operator " << type << " returning 0.0 " << std::endl;
            return 0.0;
    }
}
