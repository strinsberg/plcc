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
  Token* value = tokens.back();  // What are we doing with this
  tokens.pop_back();

  Token* type = *(tokens.rbegin() + 1);
  add_vars(type->tag, 1, line);

  tokens.pop_back();
  delete value;
  delete type;
}

void Actions::array_def(int vars, int line) {
  Token* type = *(tokens.rbegin() + vars + 1);
  Token* size = *(tokens.rbegin() + vars);  // What are we doing with this?
  add_vars(type->tag, vars, line);

  tokens.pop_back();
  tokens.pop_back();
  delete size;
  delete type;
}

void Actions::var_def(int vars, int line) {
  Token* type = *(tokens.rbegin() + vars);
  add_vars(type->tag, vars, line);

  tokens.pop_back();
  delete type;
}

void Actions::add_vars(yytokentype type, int vars, int line) {
  for (int i = 0; i < vars; i++) {
    Token* name = tokens.back();
    string lexeme = name->to_string();
    Word* w = new Word(lexeme);

    table.put( lexeme, new Id(w, type, line) );

    tokens.pop_back();
    delete name;
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
