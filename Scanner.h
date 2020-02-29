#include "Core.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

class Scanner {

  private:
  const static int NUMBER_OF_KEYWORDS = 14;
  const static int NUMBER_OF_SYMBOLS = 11;
  enum type {letter, number, symbol};
  char symbols[NUMBER_OF_SYMBOLS] = {';', '(', ')', ',', ':', '!', '=', '<', '+', '-', '*'};
  std::set<char> separators {' ', '\r', '\n', '\t'}; 
  std::string keywords[NUMBER_OF_KEYWORDS] = {"program", "begin", "end", "int", "endfunc", "if", "then",
                                  "else", "while", "endwhile", "endif", "input", "output", "or"};

  type getType(char c){
    if (((c >= 'a')&&(c <= 'z')) || ((c >= 'A')&&(c <= 'Z'))) return letter;
    else if ((c >= '0')&&(c <= '9')) return number;
    else return symbol;
  }

  std::string scanIDorKW(Core &t){
    std::stringstream temp;
    while (getType(myfile.peek()) == 0 || getType(myfile.peek()) == 1) {
      if (myfile.peek() == '0') {
        std::cout << "ERROR: No Zero is allowed in an ID" << std::endl; 
        exit(-1);
      }
      temp.put(myfile.get());
    }
    isKeyword(temp.str(), t);
    return temp.str();
  }

  std::string scanConstant(Core &t){
    std::stringstream temp;
    int digitCount = 0;
    if (getType(myfile.peek()) == number){
      char firstDigit = myfile.get();
      if (firstDigit == '0' && getType(myfile.peek()) == number) {
        std::cout << "ERROR: Leading zero detected" << std::endl; 
        exit(-1);
      }else {
        temp.put(firstDigit);
      }
    }
    while (getType(myfile.peek()) == number){
      digitCount++;
      temp.put(myfile.get());
      if (digitCount > 4) {
	    std::cout << "ERROR: Constant overflow" << std::endl; 
        exit(-1);
      }
      if (digitCount == 4) {
	      if (1023 < std::stoi(temp.str(),nullptr)) {
	        std::cout << "ERROR: Constant overflow" << std::endl; 
	        exit(-1);
	      }
      }
    }
    t = CONST;
    count = 26;
    return temp.str();
  }

  std::string scanSPSymbol(Core &t){
    std::stringstream temp;
    //std::cout << temp.str().compare("") << std::endl;
    char c = myfile.get();
    for (int i = 0; i < NUMBER_OF_SYMBOLS; ++i){
      if (symbols[i] == c){
        temp.put(c);
        count = i+14;
        if (count == 18){   //assign
          if (!(myfile.peek() == '=')) {
            std::cout << "ERROR: Encountered an undefined character: " << c << std::endl; 
            exit(-1);
          } else temp.put(myfile.get());      
        }
        if (count == 21){
          if (myfile.peek() == '=') {
            count = 25;
            temp.put(myfile.get());
          } 
        }
        break;
      }
    }
    if (temp.str().compare("") == 0) {
      std::cout << "ERROR: Encountered an undefined character: " << c << std::endl; 
      exit(-1);
    }
    return temp.str();
  }

  bool isKeyword(std::string str, Core &t){
    for (int i = 0; i < NUMBER_OF_KEYWORDS; ++i) {
      if (0 == str.compare(keywords[i])){
        count = i;
        findToken(t);
        return true;
      }
    }
    count = 27;
    findToken(t);
    return false;
  }

  void findToken(Core &t){
    switch (count) {
      case 0 : t = PROGRAM; break;
      case 1 : t = BEGIN; break;
      case 2 : t = END; break;
      case 3 : t = INT; break;
      case 4 : t = ENDFUNC; break;
      case 5 : t = IF; break;
      case 6 : t = THEN; break;
      case 7 : t = ELSE; break;
      case 8 : t = WHILE; break;
      case 9 : t = ENDWHILE; break;
      case 10 : t = ENDIF; break;
      case 11 : t = INPUT; break;
      case 12 : t = OUTPUT; break;
      case 13 : t = OR; break;
      case 14 : t = SEMICOLON; break;
      case 15 : t = LPAREN; break;
      case 16 : t = RPAREN; break;
      case 17 : t = COMMA; break;
      case 18 : t = ASSIGN; break;
      case 19 : t = NEGATION; break;
      case 20 : t = EQUAL; break;
      case 21 : t = LESS; break;     
      case 22 : t = ADD; break;
      case 23 : t = SUB; break;
      case 24 : t = MULT; break;
      case 25 : t = LESSEQUAL; break;
      case 26 : t = CONST; break;
      case 27 : t = ID; break;
      default : t = EOS; break;
    }
  }


  public: 
	int count;
	std::ifstream myfile;
	std::stringstream token;

  // Initialize the scanner
  Scanner(char* filename) {
    count = -1;
    myfile.open(filename);
    if (myfile.is_open()) {
        nextToken();
    }
    else {
      std::cout << "ERROR: Unable to open the file!";
      exit(-1);
    }
  }

  // Advance to the next token and return the token
  Core nextToken() {
    Core t = EOS;
    char c;
    token.str(""); //clear the stream
    while(separators.find(myfile.peek()) != separators.end())
      myfile.ignore(1);
    c = myfile.peek();
    if (c == -1){
        count = -1;
        return EOS;
    }
    //std::cout << (int)c << std::endl;
    type flag = getType(c);
    switch (flag)
    {
      case letter: token << scanIDorKW(t); break;
      case number: token << scanConstant(t); break;
      case symbol: token << scanSPSymbol(t); break;
      default: std::cout << "ERROR: Encountered an undefined character: " << c << std::endl; exit(-1);
    }
    return t;
  }

  // Return the current token
  Core currentToken() {
    // Just returning some values for demonstration purposes
    Core t;
    findToken(t);
    return t;
  }

  std::string getID() {
    // Returning a value for demonstration purposes
    return token.str();
  }

  int getCONST() {
    // Returning a value for demonstration purposes
    return std::stoi(token.str(),nullptr);
  }

  void match(int token) {
    if (token == currentToken()) {
      //std::cout << currentToken() << std::endl;
      nextToken();
    }
    else {
      std::cout << "ERROR: You have an unmatched token: "<< token <<" "<< currentToken() << std::endl;
      if (token == 11) std::cout << "ERROR: Missing SemiColon "<< std::endl;
      std::cout << "ERROR: Invalid Syntax "<< std::endl;
      exit(-1);
    }
  }
};
 
