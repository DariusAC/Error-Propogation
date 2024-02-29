# Error-Propogation

Compiled with GNU C++ 23

This program parses equations, evalutes them and approximates error propogation.

Ex. Usage in Interactive Use
./err_analysis.out

Enter the formula for f: 2*x+4*y<br>
Set variable x: 0.1<br>
Set precision for x: 0.00001<br>
Set uncertainty for x: 0.1<br>
Set variable y: 0.2<br>
Set precision for y: 0.0001<br>
Set uncertainty for y: 0.5<br>

Result is 1<br>
Uncertainty is 2.00998

Ex. Usage with options<br>
./err_analysis.out -if "Example_Table/in.dat" -of "Example_Table/out.dat" -eq "(m+n)/(m-n)*(2*l/t^2)"

See Example_Table/in.dat and Example_Table/out.dat for the respective output
