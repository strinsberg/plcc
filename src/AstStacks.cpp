#include "AstStacks.h"
#include "Tokens.h"
#include "Exprs.h"
#include "Stmts.h"
#include "Defs.h"
#include <iostream>
using namespace std;

AstStacks::AstStacks() {}

AstStacks::~AstStacks() {
  for (auto & t : tokens)
    delete t;

  for (auto & e : exprs)
    delete e;

  for (auto & s : stmts)
    delete s;
}

// Token addition /////////////////////////////////////////////////////
void AstStacks::push_token(tag::Tag t) {
  tokens.push_back(new Token(t));
}

void AstStacks::push_word(std::string s) {
  tokens.push_back(new Word(s));
}

void AstStacks::push_num(int n) {
  tokens.push_back(new Number(n));
}

void AstStacks::push_float(int n, int d) {
  tokens.push_back(new Float(n, d));
}

void AstStacks::push_char(char c) {
  tokens.push_back(new Char(c));
}

// Pop Nodes /////////////////////////////////////////////////////////

// WARNING: This token is not removed and must be popped at the right time!
// Should probably make it erase it, as it will only be used with a few
// things on the stack max.
Token* AstStacks::get_token(int from_top) {
  return *(tokens.rbegin() + from_top);
}

Token* AstStacks::pop_token() {
  if (tokens.size() > 0) {
    Token* next = tokens.back();
    tokens.pop_back();
    return next;
  }
  return new Token(tag::EMPTY);
}

Expr* AstStacks::pop_expr() {
  if (exprs.size() > 0) {
    Expr* next = exprs.back();
    exprs.pop_back();
    return next;
  }
  return new Expr( new Token(tag::EMPTY), tag::EMPTY, -1 );
}

Stmt* AstStacks::pop_stmt() {
  if (stmts.size() > 0) {
    Stmt* next = stmts.back();
    stmts.pop_back();
    return next;
  }
  return new Stmt(-1);
}

Def* AstStacks::pop_def() {
  if (defs.size() > 0) {
    Def* next = defs.back();
    defs.pop_back();
    return next;
  }
  return new Def(-1);
}


// Display stacks /////////////////////////////////////////////////////

void AstStacks::print_tokens() {
  cout << endl;
  cout << "=== Token Stack ===" << endl;
  for (auto & t : tokens) {
    cout << t->to_string() << endl;
  }
}

void AstStacks::print_nodes() {
  cout << endl;
  cout << "=== Definition Nodes: " << defs.size() << " ===" << endl;
  for (auto & d : defs) {
    cout << d->to_string() << endl;
  }

  cout << endl;
  cout << "=== Statment Nodes: "<< stmts.size() << " ===" << endl;
  for (auto & s : stmts) {
    cout << s->to_string() << endl;
  }

  cout << endl;
  cout << "=== Expression Nodes: " << exprs.size() << " ===" << endl;
  for (auto & e : exprs) {
    cout << e->to_string() << endl;
  }
}
