#include "Actions.h"
#include <string>
#include <iostream>

using namespace std;


Actions::~Actions() {
  for (auto & t : tokens)
    delete t;

  for (auto & n : nodes)
    delete n;
}


// Token addition /////////////////////////////////////////////////////
void Actions::add_t(yytokentype t) {
  tokens.push_back(new Token(t));
}

void Actions::add_w(std::string s) {
  tokens.push_back(new Word(s));
}

void Actions::add_n(int n) {
  tokens.push_back(new Number(n));
}

void Actions::add_f(int n, int d) {
  tokens.push_back(new Float(n, d));
}

void Actions::add_c(char c) {
  tokens.push_back(new Char(c));
}


// AstNode addition ///////////////////////////////////////////////////
// Or should there just be rule functions that take care of all the
// work in an associated rule??? This could get really big if we do that
// by the time the program grows. But otherwise all the logic will have to
// go inside the action brackets in the rules and I would not like that.


// Display methods ////////////////////////////////////////////////////
void Actions::print_tokens() {
  cout << endl;
  cout << "=== Token Stack ===" << endl;
  for (auto & t : tokens) {
    cout << t->to_string() << endl;
  }
}

void Actions::print_nodes() {

}

