#include "Function.hpp"

std::string Function::operators = "^/*-+";

// Initialize a function from a string equation
Function::Function(std::string equation) {
	// Corrects for negatives, whitespace, and functions without operations
	bool no_operations = true;

	// Change -x to 0-x at start of equation
	if (equation.at(0) == '-')
		equation = "0" + equation;

	for (uint16_t i = 0 ; i < equation.length() ; i++) {
		char c = equation.at(i);
		if (operators.contains(c)) {
			no_operations = false;
			// Change every -x to (0-x)
			if (equation.at(i) == '-' && equation.at(i-1) == '(') {
				uint16_t j = 1;
				while (!operators.contains(equation.at(i+j)) && i+j < equation.length() && equation.at(i+j) != ')')
					j++;
				equation.replace(i, j, "(0-" + equation.substr(i+1, j-1) + ")");
			}
		}else if (c == ' ') {
			// Remove whitespace
			equation.erase(i, 1);
			i--;
		}
	}
	// Add at least one operation
	if (no_operations)
		equation += "+0";

	size = 1;                                                  // Number of operations
	char cp = '+';                                             // Immediate previous charecter in equation
	int16_t p = 0;                                             // Running count of parentheses
	int16_t mp = 0;                                            // Max number of parentheses

	// Syntax checker and counts how many layers of parentheses there are
	for (uint16_t i = 0 ; i < equation.length() ; i++) {
		char c = equation.at(i);

		// Check for incorrect operator placement
		if (operators.contains(c)) {
			if (cp == '(') {
				std::cerr << "Syntax Error: cannot follow parenthesis with an operator: " << c << std::endl;
				return;
			}
			else if (operators.contains(cp)) {
				std::cerr << "Syntax Error: cannot have two operators in a row: " << c << " " << cp << std::endl;
				return;
			}
			size += 2;                                               // Two operations for every operator
		}
		// Check for incorrect variable names/placement
		else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
			if ((cp >= 'a' && cp <= 'z') || (cp >= 'A' && cp <= 'Z')) {
				std::cerr << "Syntax Error: variables names restircted to one letter, found: " << c << cp << std::endl;
				return;
			}else if (!operators.contains(cp) && cp != '(') {
				std::cerr << "Syntax Error: invalid preceding charecter to variable: " << c << " " << cp << std::endl;
			}
		}
		// Check parentheses count and syntax
		else if (c == '(') {
			if (cp != '(' && !operators.contains(cp)) {
				std::cerr << "Syntax Error: invalid charecter before (: " << cp << std::endl;
				return;
			}
			p++;
			if (p > mp)
				mp = p;
		}
		else if (c == ')') {
			if (cp != ')' && cp < '0' && cp > '9' && ((cp < 'a' || cp > 'z') && (cp < 'A' || cp > 'Z'))) {
				std::cerr << "Syntax Error: invalid charecter before ): " << cp << std::endl;
				return;
			}
			else if (p == 0) {
				std::cerr << "Syntax Error: unmatched closing parenthesis" << std::endl;
				return;
			}
			p--;
		}
		// Check for unrecognized charecters
		else if((c < '0' || c > '9') && c != '.') {
			std::cerr << "Syntax Error: unrecognized charecter: " << c << std::endl;
			return;
		}

		cp = c;
	}
	if (p != 0) {
		std::cerr << "Synatx Error: " << p << " unmatched opening parentheses" << std::endl;
		return;
	}

	f = (Operation*)malloc(size*sizeof(Operation));
	initialized = (bool*)malloc(size);
	memset(initialized, false, size);

	required_variables = "";

	uint8_t op_index = size-1; // Building f from back to front. Indexes current location in array

	// Start by building our binary tree in reverse order of operations, starting with the innermost parentheses
	for (int8_t i = mp ; i >= 0 ; i--) {
		uint8_t pos = 0;
		p = 0;
		// Run through the equation at a given parenthesis depth. length > 2 ensure no endless loops when finished
		while (pos < equation.length() && equation.length() > 2) {
			uint8_t a = 0;                                           // Indexes begining of expression
			uint8_t b = 0;                                           // Indexes end of expression

			std::string expression = "";

			if (i != 0) {                                            // Find inside of parentheses
				while (p != i && pos < equation.length()) {
					char c = equation.at(pos);
					if (c == '(')
						p++;
					else if (c == ')')
						p--;
					pos++;
				}
				a = pos - 1;

				while (p >= i && pos < equation.length()) {
					char c = equation.at(pos);
					expression += c;
					if (c == '(')
						p++;
					else if (c == ')')
						p--;
					pos++;
				}
				b = pos;
				// If at end of the string, move on
				if (b - a == 1)
					continue;
				expression = equation.substr(a+1, b-a-2);
			}else {                                                  // No parentheses left, expression is the whole equation
				a = 0;
				b = expression.length()-1;
				expression = equation;
			}
			// Construct operations from expressions
			for (auto &o : operators) {
				std::string::iterator it, op_start, op_end;
				for (it = expression.begin(); it < expression.end() ; it++) {
					if (*it == o) {
						// Move in both directions from the operator to find its operands
						op_start = it - 1;
						op_end = it + 1;

						uint8_t op_index_correction = 0;                      // This variable marks how many operations have been created
						uint8_t o1, o2;                                       // These variables mark where the operands's Operator is to link back to the main Operator

						// Work backwards to first operand
						while (((*op_start >= '0' && *op_start <= '9') || *op_start == '.') && op_start > expression.begin())
							op_start--;
						if (operators.contains(*op_start))
							op_start++;
						// Set first operation
						if (*op_start == '@') {
							// Referencing previous operator
							op_start++;
							o1 = std::stoi(expression.substr(op_start-expression.begin(), it-op_start));
							op_start--;
						}else if ((*op_start >= 'a' && *op_start <= 'z') || (*op_start >= 'A' && *op_start <= 'Z')) {
							// Referencing variable
							if (it - op_start != 1) {
								std::cerr << "Parser Error: syntax should have ensured single charecter variable names, instead found: " << expression.substr(expression.begin() - op_start, it-op_start) << std::endl;
								return;
							}
							if (!required_variables.contains(*op_start))
								required_variables += *op_start;

							o1 = op_index;

							InitializeOperation(op_index, '=', 0, *op_start);
							op_index_correction++;
						}else if ((*op_start >= '0' && *op_start <= '9') || *op_start == '.') {
							// Referencing constant
							double v = std::stod(expression.substr(op_start-expression.begin(), it-op_start));
							o1 = op_index;

							InitializeOperation(o1, '=', v, 0);
							op_index_correction++;
						}else {
							std::cerr << "Parser Error: unhandled charecter in parser: " << (int)*op_start << " at " << (int)(op_start-it) << " in " << expression << std::endl;
						}
						// Work forwards to second operand
						if (*op_end == '@') {
							// Referencing previous operator
							op_end++;

							while (*op_end >= '0' && *op_end <= '9' && op_end < expression.end())
								op_end++;
							op_end--;

							// Grab the index of the operator refered to
							o2 = std::stoi(expression.substr(it - expression.begin() + 2, op_end - it - 1));
						}else if ((*op_end >= 'a' && *op_end <= 'z') || (*op_end >= 'A' && *op_end <= 'Z')) {
							// Referencing variable
							if (!required_variables.contains(*op_end))
								required_variables += *op_end;

							o2 = op_index-op_index_correction;

							InitializeOperation(o2,'=', 0, *op_end);
							op_index_correction++;
						}else if ((*op_end >= '0' && *op_end <= '9') || *op_end == '.') {
							// Referencing constant
							while (((*op_end >= '0' && *op_end <= '9') || *op_end == '.') && op_end < expression.end())
								op_end++;
							op_end--;

							double v = std::stod(expression.substr(it-expression.begin()+1, op_end-it));

							o2 = op_index-op_index_correction;

							InitializeOperation(o2, '=', v, 0);
							op_index_correction++;
						}else {
							std::cerr << "Unrecognized charecter " << *op_end << " in expression " << expression << std::endl;
							std::cerr << "For reference, it is at " << *it << std::endl;
							std::cerr << "For reference, it + 1 is: " << *(it+1) << std::endl;
						}

						// Link both operands with operator
						InitializeOperation(op_index-op_index_correction, o, 0, 0);
						f[op_index-op_index_correction].o1 = &f[o1];
						f[op_index-op_index_correction].o2 = &f[o2];

						op_index -= op_index_correction;

						// Substitute expression with reference to operator
						std::string tag = "@" + std::to_string((int)op_index);
						expression.replace(op_start-expression.begin(), op_end-op_start+1, tag);

						pos = expression.find(tag)+tag.length();

						op_index--;
					}
				}
			}
			// Replace expression in equation with reference to operator
			equation.replace(a, b-a, expression);
			pos = equation.find(expression);
		}
	}

	// Check if the function was generated properly
	for (uint16_t i = 0 ; i < size ; i++) {
		if (!initialized[i]) {
			std::cout << "Error: Function improperly initialized! @" << i << " not initialied!" << std::endl;
			return;
		}
	}
}
double Function::Evaluate(Variables * x) {
	for (uint16_t i = 0 ; i < size ; i++) {
		if (!initialized[i]) {
			std::cerr << "Error: Cannot evaluate function. Operation @" << i << " undefined! Returning 0.0..." << std::endl;
			return 0.0;
		}
	}
	return f->Evaluate(x);
}
double Function::PartialDerivative(Variables * x, char c, double percision) {
	if (!required_variables.contains(c)) {
		std::cerr << "Error: Cannot do partial derivative on variable " << c << " since it isn't used by f! Returning 0.0..." << std::endl;
		return 0.0;
	}
	double y1 = Evaluate(x);
	double x_initial = x->GetVar(c);
	x->SetVar(c, x_initial+percision);

	double y2 = Evaluate(x);
	x->SetVar(c, x_initial);

	return (y2-y1)/percision;
}
double Function::PropogateError(Variables * x, Variables * uncertainty, Variables * percision) {
	double innerSum = 0.0;
	for (auto &c : GetRequiredVariables()) {
		innerSum += pow(PartialDerivative(x, c, percision->GetVar(c))*uncertainty->GetVar(c),2);
	}
	return sqrt(innerSum);
}
void Function::InitializeOperation(uint16_t i, char type, double value = 0, char variable = 0) {
	if (initialized[i]) {
		std::cerr << "Error: Trying to initialize operation @" << i << " that has already been set!" << std::endl;
		return;
	}
	f[i] = Operation(type, value, variable);
	initialized[i] = true;
}
