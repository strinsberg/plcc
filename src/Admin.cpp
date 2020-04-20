#include "Admin.h"
#include <iostream>
#include <string>
using namespace std;


Admin::Admin(bool d) : line(1), errors(0), line_error(false),  is_debug(d) {};
Admin::~Admin() {};


void Admin::debug(std::string text) {
  if (is_debug)
    cout << text << endl;
}


void Admin::error(std::string text, std::string lexeme) {
  if (!line_error) {
    line_error = true;
    
    cerr << "error on line " << line;
    if (lexeme != "")
      cerr << " near '" << lexeme << "'";
    cerr << ": " << text << endl;
  }
  errors++;
}


void Admin::newline() {
  line++;
  line_error = false;
}


int Admin::get_line() {
  return line;
}


int Admin::get_errors() {
  return errors;
}

