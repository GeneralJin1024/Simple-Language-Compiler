Shengyu Jin (jin.1024)

Files: Core.h/ Scanner.h/ Parser.h/ main.cpp/ README.txt 
       Scanner.h is the code from my last project and an extra method added. Scanner.match(token) is the method that checks the
       current token, if matching then consumes, otherwise throw "ERROR: Invalid Syntax". That is saying, missing a semicolon, a comma or
       a keyword can be a syntax and this error message can be thrown out. 
       
       Parser.h is the code that I implemented for this project. Using the object-oriented way to construct a parser
       tree. A separate class for each non-terminal X. A memory class simulates the function stack of the program.      

Features: Missing a semicolon, a comma or a keyword can be a syntax error and throw massages like "ERROR: You have an unmatched token", "ERROR: Invalid Syntax".

Compile command: g++ -std=c++11 main.cpp -o main
Run command: ./main <inputFileName>

The answer is printing in the command line.
