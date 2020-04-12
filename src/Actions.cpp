#include "Actions.h"
#include <string>
#include <iostream>

using namespace std;


Actions::~Actions() {
  for (auto & t : tokens)
    delete t;

  for (auto & e : exprs)
    delete e;

  for (auto & s : stmts)
    delete s;
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
void Actions::const_def() {
  
}

// Has duplicate code and should have an array ID type?
// Both still need some kind of error handling?
void Actions::array_def(int vars, int line) {
  vector<Token*> names;
  for (int i = 0; i < vars; i++) {
    names.push_back( tokens.back() );
    tokens.pop_back();
  }

  Token* size = tokens.back();
  tokens.pop_back();

  Token* type = tokens.back();
  tokens.pop_back();

  for (int i = 0; i < vars; i++) {
    Word* w = new Word( names[i]->to_string() );
    exprs.push_back( new Id(w, type->tag, line) );
    delete names[i];
  }

  delete size;
  delete type;
}

void Actions::var_def(int vars, int line) {
  vector<Token*> names;
  for (int i = 0; i < vars; i++) {
    names.push_back( tokens.back() );
    tokens.pop_back();
  }

  Token* type = tokens.back();
  tokens.pop_back();
  delete type;

  for (int i = 0; i < vars; i++) {
    Word* w = new Word( names[i]->to_string() );
    exprs.push_back( new Id(w, type->tag, line) );
    delete names[i];
  }
}


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

