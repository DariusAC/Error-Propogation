#ifndef OPERATION_HPP
#define OPERATION_HPP

#include <iostream>
#include <cmath>

#include "Variables.hpp"


class Operation {
private:
	// What operation will be executed? e.g. +, -, =, ^, etc.
	char type;

public:
	// What operations is this one linked to? e.g. (2+3) + (5+4) is two operations linked together
	Operation * o1;
	Operation * o2;

	double value;
	char var;

	Operation(char type, double value, char var) : type(type), value(value), var(var) {}

	double Evaluate(Variables * x);
};

#endif
