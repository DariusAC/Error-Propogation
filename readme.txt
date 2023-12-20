This program parses equations and evalutes them and performs error propogation.

Ex. Usage in Interactive Use
./err_analysis.out

Enter the formula for f: 2*x+4*y
Set variable x: 0.1
Set precision for x: 0.00001
Set uncertainty for x: 0.1
Set variable y: 0.2
Set precision for y: 0.0001
Set uncertainty for y: 0.5

Result is 1
Uncertainty is 2.00998

Ex. Usage with options
./err_analysis.out -if "Example_Table/in.dat" -of "Example_Table/out.dat" -eq "(m+n)/(m-n)*(2*l/t^2)"

See respective files for input/output results
