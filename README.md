# PL/0 Compiler

Project for Systems Software class. This compiler reads in a program written in PL/0 and generates code for the Virtual Machine included. It also ensures that it doesn't parse nor generate code for programming constructs that are not in the grammar outlined for the project.

A demonstration of how the parser handles all possible errors from different input files to the scanner can be found in ```outputs.docx```.

#### Compile

To compile run ```gcc main.c Lexical_Analyzer.c VM.c```

#### Usage

To run the program on Linux, the file name must be followed by ```-f```. Include any or all of the compiler directives, ```-l -a -v``` after the filename.

Example:
```
./a.out -f test.txt -l -a -v
```

To route all output to a text file use
```
./a.out -a -l -v -f <filename> > output.txt
```

Assembly output for the Virtual Machine is placed in the file Assembly.txt

#### Command List

* ```-a``` prints assembly output

* ```-l``` prints lexical analyzer output

* ```-v``` prints virtual machine output

* ```-f [filename]``` specify a filename to be the input

#### Authors
Austin Peace, Sebastian De La Cruz
