#ifndef VARIABLES_HPP
#define VARIABLES_HPP

#include <string>
#include <cstdlib>
#include <cstdint>
#include <iostream>

class Variables {
private:
	double * values;
	std::string keys;
public:
	Variables();
	~Variables();

	std::string GetKeys() { return keys; }
	void SetVar(char key, double v);
	double GetVar(char key);
};

#endif
