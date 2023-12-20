#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "Variables.hpp"
#include "Function.hpp"

// This runs if no parameters are passed. User types in a formula, e.g. 9.81*t^2/2
// Then for every variable in the equation, the user is prompted to enter a value, uncertainty, and desired precision
// e.g. t = 4.2, ds = 0.1, precision = 0.00001
void RunInteractive() {
	Variables variables, percision, uncertainty;

	std::string equation;

	std::cout << "Enter the formula for f: ";
	std::getline(std::cin, equation);

	Function f(equation);

	for (auto &c : f.GetRequiredVariables()) {
		double v;

		std::cout << "Set variable " << c << ": ";
		std::cin >> v;

		variables.SetVar(c, v);

		std::cout << "Set percision for " << c << ": ";
		std::cin >> v;

		percision.SetVar(c, v);

		std::cout << "Set uncertainty for " << c << ": ";
		std::cin >> v;

		uncertainty.SetVar(c, v);

	}

	std::cout << "Result is " << f.Evaluate(&variables) << std::endl;
	std::cout << "Uncertainty is " << f.PropogateError(&variables, &uncertainty, &percision) << std::endl;
}

int main(int argc, char **argv) {
	if (argc == 1) {
		RunInteractive();
		return 0;
	}

	// If not running interactive, read from specified file
	// NOTE: WIP, slight errors still with last column in file. Just add a throwaway column and avoid extra whitespace	
	// 	otherwise fully functional

	std::string equation = "";

	std::string input_path = "";
	std::string output_path = "out.txt";

	std::ifstream input_stream;
	std::ofstream output_stream;

	// Extract options from CLI command
	for (uint16_t i = 1 ; i < argc ; i++) {
		std::string flag = std::string(argv[i]);
		if (flag == "-if") {
			if (i + 1 == argc) {
				std::cerr << "Error: No path supplied for input file" << std::endl;
				input_stream.close();
				output_stream.close();
				return -1;
			}else if (argv[i+1][0] == '-') {
				std::cerr << "Error: Option supplied instead of path for input file" << std::endl;
				input_stream.close();
				output_stream.close();
				return -1;
			}
			input_path = argv[i+1];
			input_stream.open(input_path);
			if (!input_stream.is_open()) {
				std::cerr << "Error: Failed to open input file" << input_path << std::endl;
				output_stream.close();
				return -1;
			}
			i++; // iterate over both the option and the name of the file
		} else if (flag == "-of") {
			if (output_stream.is_open()) {
				std::cerr << "Error: output stream already open" << std::endl;
				output_stream.close();
				input_stream.close();
				return -1;
			}
			if (i + 1 == argc) {
				std::cerr << "Error: No path supplied for output file" << std::endl;
				input_stream.close();
				return -1;
			}else if (argv[i+1][0] == '-') {
				std::cerr << "Error: Option supplied instead of path for output file" << std::endl;
				input_stream.close();
				return -1;
			}
			output_path = argv[i+1];
			output_stream.open(output_path);
			if (!output_stream) {
				std::cerr << "Error: Failed to open output file" << output_path << std::endl;
				input_stream.close();
				return -1;
			}
			i++; // iterate over both the option and the name of the file
		} else if (flag == "-eq") {
			if (i + 1 == argc) {
				std::cerr << "Error: No equation provided for -eq!" << std::endl;
				input_stream.close();
				output_stream.close();
				return -1;
			}else if (argv[i+1][0] == '-') {
				std::cerr << "Error: Option supplied instead of equation" << std::endl;
				input_stream.close();
				output_stream.close();
				return -1;
			}
			equation = (std::string)argv[i+1];
			i++; // iterate over both the option and the string for the equation
		} else {
			std::cerr << "Unrecognized option " << flag << std::endl;
		}

	}
	// If output stream not yet opened, use default
	if (!output_stream) {
		output_stream.open(output_path);
		if (!output_stream) {
			std::cerr << "Error: Failed to open default output file" << output_path << std::endl;
			input_stream.close();
			return -1;
		}
	}
	if (input_path == "") {
		std::cerr << "Error: No input path supplied" << std::endl;
		return -1;
	}
	// Once everything is open, begin processing
	Function f(equation);

	std::string variable_label;
	std::string required_variables = f.GetRequiredVariables();
	std::getline(input_stream, variable_label);

	// Ensure the table will define every variable
	for (auto &c : required_variables) {
		if (!variable_label.contains(c)) {
			std::cerr << "Input table does not define required variable " << c << " for f = " << equation << std::endl;

			input_stream.close();
			output_stream.close();
			return -1;
		}
	}

	// Clean table
		
	uint16_t table_width = 1;
	
	variable_label.erase(variable_label.find_last_not_of(' ')+1);
	variable_label.erase(variable_label.find_last_not_of('	')+1);

	for (uint16_t i = 1 ; i < variable_label.length() ; i++) 
		if ((variable_label.at(i) == ' ' || variable_label.at(i) == '	') && (variable_label.at(i-1) != ' ' && variable_label.at(i-1) != '	'))
			table_width++;

	uint16_t ** column_index = (uint16_t **)malloc(required_variables.length()*sizeof(uint16_t*));
	for (uint16_t i = 0 ; i < required_variables.length() ; i++)
		column_index[i] = (uint16_t *)malloc(3*sizeof(uint16_t));

	uint16_t current_index = 0;
	uint16_t a = 0; // marks start of a variable name for substring
			
	// Creates a list of where the relavent column headers are
	for (uint16_t i = 0 ; i < variable_label.length() ; i++) {
		std::cout << i << std::endl;
		if (variable_label.at(i) == ' ' || variable_label.at(i) == '	' || i == variable_label.length() - 1) {
			std::string var_name;
			if (i == variable_label.length() - 1) {
				var_name =  variable_label.substr(a);
				a--;
			}else {
				var_name = variable_label.substr(a, i-a);
			}
			if (i - a == 1) {
				if (required_variables.contains(var_name)) {
					column_index[required_variables.find(var_name)][0] = current_index;
				}
			}else if (i - a == 2) {
				if (required_variables.contains(var_name.at(1))) {
					if (var_name.at(0) == 'd')
						column_index[required_variables.find(var_name.at(1))][1] = current_index;
					else if (var_name.at(0) == 'p')
						column_index[required_variables.find(var_name.at(1))][2] = current_index;
				}
			}
			while (variable_label.at(i) == ' ' || variable_label.at(i) == '	' && i < variable_label.length())
				i++;
			
			a = i;
			current_index++;
		}	
	}

	std::vector<std::vector<Variables>> x;

	// This buffer will contain the final output until we're ready to write it to file
	std::stringstream buffer;
	buffer << variable_label << std::endl;
	
	std::string line;
	double * trial_results = (double *)malloc(table_width*sizeof(double)); // This will store all of the outputs from applying the function to the given values in the table
	while (std::getline(input_stream, line)) {
		buffer << line << std::endl;

		uint16_t i = 0;	
		uint16_t index = 0;

		// Create an empty set of variables for the next trial
		x.push_back(std::vector<Variables>());
		x.back().emplace_back();
		x.back().emplace_back();
		x.back().emplace_back();

		// Then populate with variables...
		for (uint16_t j = 0 ; j < line.length() ; j++) {
			double value;
			if (line.at(i) == ' ') {
				if (line.at(j) != ' ' && line.at(j) != '	' && j != line.length() - 1)
					i = j;
				continue;
			}
			else if (line.at(j) == ' ' || line.at(j) == '	') {
				value = std::stod(line.substr(i, j-i));
				i = j;
			}else if (j == line.length() - 1)
				value = std::stod(line.substr(i, j-i+1));
			else
				continue;
			for (uint16_t k = 0 ; k < required_variables.length() ; k++) {
				if (index == column_index[k][0])
					x.back()[0].SetVar(required_variables.at(k), value);
				else if (index == column_index[k][1])
					x.back()[1].SetVar(required_variables.at(k), value);
				else if (index == column_index[k][2])
					x.back()[2].SetVar(required_variables.at(k), value);
			}
			index++;
		}
	}

	// Test read. Debugging purposes...
	for (uint16_t i = 0 ; i < x.size() ; i++) {
		std::cout << std::endl << std::endl;
		std::cout << "For trial " << i << " variables:" << std::endl;
		for (auto &c : required_variables) {
			std::cout << c << ": " << x[i][0].GetVar(c) << std::endl;
			std::cout << "d" << c << ": " << x[i][1].GetVar(c) << std::endl;
			std::cout << "p" << c << ": " << x[i][2].GetVar(c) << std::endl;
		}
	}
	std::cout << std::endl << std::endl;
	
	// Iterate over the buffer and append results of applying the function and calculating error
	std::getline(buffer, line);
	output_stream << line << "	res	err" << std::endl;
	std::cout << line << std::endl;
	
	for (uint16_t i = 0 ; i < x.size() ; i++) { 
		std::getline(buffer, line);
		std::cout << line << std::endl;
		output_stream << line << "	" << f.Evaluate(&x[i][0]) << "	" << f.PropogateError(&x[i][0], &x[i][1], &x[i][2]) << std::endl;
	}	
	// Write output then cleanup

	output_stream.close();
	input_stream.close();

	delete trial_results;

	for (uint8_t i = 0 ; i < required_variables.length() ; i++)
		delete column_index[i];
	delete column_index;
	return 0;
}
