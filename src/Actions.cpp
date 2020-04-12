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


// Definitions ///////////////////////////////////////////////////

// Doesn't do anything with the value
// Also none of the definitions actually deal with the variable other than
// buidling the nodes for the tree
void Actions::const_def(int line) {
  Token* value = tokens.back();
  tokens.pop_back();

  Token* name = tokens.back();
  tokens.pop_back();

  Token* type = tokens.back();
  tokens.pop_back();
  
  Word* w = new Word(name->to_string());
  exprs.push_back( new Id(w, type->tag, line) );

  //delete value;
  delete name;
  delete type;
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

  for (int i = 0; i < vars; i++) {
    Word* w = new Word( names[i]->to_string() );
    exprs.push_back( new Id(w, type->tag, line) );
    delete names[i];
  }

  delete type;
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
  cout << endl;
  cout << "=== Statment Nodes ===" << endl;
  for (auto & s : stmts) {
    cout << s->to_string() << endl;
  }

  cout << endl;
  cout << "=== Expression Nodes ===" << endl;
  for (auto & e : exprs) {
    cout << e->to_string() << endl;
  }
}

