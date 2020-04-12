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

void Actions::const_def(int line) {
  // Do we need to store this value anywhere? or create an Expr?
  Token* value = tokens.back();
  tokens.pop_back();

  Token* name = tokens.back();
  tokens.pop_back();

  Token* type = tokens.back();
  tokens.pop_back();
  
  Word* w = new Word(name->to_string());
  table.put( name->to_string(), new Id(w, type->tag, line) );

  delete value;
  delete name;
  delete type;
}

void Actions::array_def(int vars, int line) {
  vector<Token*> names;
  for (int i = 0; i < vars; i++) {
    names.push_back( tokens.back() );
    tokens.pop_back();
  }

  // Not sure what to do with the size. Perhaps it needs an expression.
  Token* size = tokens.back();
  tokens.pop_back();

  Token* type = tokens.back();
  tokens.pop_back();

  for (int i = 0; i < vars; i++) {
    Word* w = new Word( names[i]->to_string() );
    table.put( names[i]->to_string(), new Id(w, type->tag, line) );
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
    table.put( names[i]->to_string(), new Id(w, type->tag, line) );
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

void Actions::print_table() {
  cout << endl;
  cout << "=== BlockTable Nodes ===" << endl;
  table.print();
}
