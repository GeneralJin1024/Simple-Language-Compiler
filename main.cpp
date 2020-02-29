#include <iostream>
#include <string>
#include "Parser.h"
//#include "Core.h"

Scanner* Parser::S = NULL;
Scanner* Parser::Input = NULL;
Func* Memory::activeFunc = NULL;
std::stack<Func*> Memory::funcStack;
std::vector<Func*> Memory::funcList;

// Compile with the command "g++ main.cpp"
// Run with the command "a.out FILENAME"
int main(int argc,char* argv[]) {

    // Initialize the scanner with the input file
  Parser::S = new Scanner(argv[1]);
  Parser::Input = new Scanner(argv[2]);
  Memory::addFunc(new Func("program"));
  Prog* Pro = new Prog();
  Pro->parse();
  Pro->print();
  Pro->exec();
}
 
