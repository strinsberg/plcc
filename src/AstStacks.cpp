#include "AstStacks.h"
#include "Types.h"
#include "Exprs.h"
#include "Stmts.h"
#include "Defs.h"
#include <iostream>
using namespace std;

AstStacks::AstStacks() {}

AstStacks::~AstStacks() {
  for (auto & e : exprs)
    delete e;

  for (auto & s : stmts)
    delete s;
}


// Type ///////////////////////////////////////////////////////////////
void AstStacks::set_type(Type t) {
  type = t;
}

Type AstStacks::get_type() {
  return type;
}


// Pop Nodes /////////////////////////////////////////////////////////
Operator AstStacks::pop_op() {
  if (ops.size() > 0) {
    Operator next = ops.back();
    ops.pop_back();
    return next;
  }
  return Operator();
}

Expr* AstStacks::pop_expr() {
  if (exprs.size() > 0) {
    Expr* next = exprs.back();
    exprs.pop_back();
    return next;
  }
  return new Expr(Type());
}

Stmt* AstStacks::pop_stmt() {
  if (stmts.size() > 0) {
    Stmt* next = stmts.back();
    stmts.pop_back();
    return next;
  }
  return new Stmt();
}

Def* AstStacks::pop_def() {
  if (defs.size() > 0) {
    Def* next = defs.back();
    defs.pop_back();
    return next;
  }
  return new Def();
}


// Display stacks /////////////////////////////////////////////////////

void AstStacks::print_nodes() {
  cout << endl;
  cout << "=================== STACKS ========================" << endl;
  cout << "Last Type:     " << symbol::str(type.type) << endl;

  cout << endl;
  cout << "=== Operator Nodes: " << defs.size() << " ===" << endl;
  for (auto & o : ops) {
    cout << symbol::str(o.op) << endl;
  }

  cout << endl;
  cout << "=== Definition Nodes: " << defs.size() << " ===" << endl;
  for (auto & d : defs) {
    cout << *d << endl;
  }

  cout << endl;
  cout << "=== Statment Nodes: "<< stmts.size() << " ===" << endl;
  for (auto & s : stmts) {
    cout << *s << endl;
  }

  cout << endl;
  cout << "=== Expression Nodes: " << exprs.size() << " ===" << endl;
  for (auto & e : exprs) {
    cout << *e << endl;
  }
}
