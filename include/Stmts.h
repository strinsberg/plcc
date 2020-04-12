#ifndef PLCC_STATEMENTS_H
#define PLCC_STATEMENTS_H

#include "AstNode.h"
#include "Exprs.h"
#include <string>


class Stmt : public AstNode {
 public:
  Stmt(int line) : AstNode(line) {}
  virtual ~Stmt() {}
  virtual std::string to_string() { return "stmt: " + std::to_string(line_num); }
};

class Seq : public Stmt {
 public:
  Seq(Stmt* f, Stmt* r, int line) : Stmt(line), first(f), rest(r) {}
  ~Seq() { if (first != nullptr) delete first; if (rest != nullptr) delete rest; }
  std::string to_string() { return first->to_string() + "\n" + rest->to_string(); }
  Stmt* first;
  Stmt* rest;
};

class Asgn : public Stmt {
 public:
  Asgn(Id* i, Expr* e, int line) : Stmt(line), id(i), expr(e) {}
  ~Asgn() { if (expr != nullptr) delete expr; } 
  std::string to_string() { return "asignment: " + id->op->to_string() + " := " + expr->op->to_string(); }
  Id* id;
  Expr* expr;
};
// if, loop, assign, write, skip
// condition?
// array assignment
// sequence of statments

#endif

