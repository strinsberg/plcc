#ifndef PLCC_STATEMENTS_H
#define PLCC_STATEMENTS_H

#include "AstNode.h"
#include "Exprs.h"
#include "Defs.h"
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

class Block : public Stmt {
 public:
  Block(Def* d, Stmt* s, int line) : Stmt(line), defs(d), stmts(s) {}
  ~Block() { if (defs != nullptr) delete defs; if (stmts != nullptr) delete stmts; }
  std::string to_string() { return "\nBlock (" + std::to_string(line_num) + "):\n--Defs--\n" + defs->to_string() + "\n--Stmts--\n" + stmts->to_string() + "\n"; }

  Def* defs;
  Stmt* stmts;
};

class Asgn : public Stmt {
 public:
  Asgn(Expr* a, Expr* e, int line) : Stmt(line), acs(a), expr(e) {}
  ~Asgn() { delete acs; delete expr; } 
  std::string to_string() { return "assignment: " + acs->to_string() + " := " + expr->to_string(); }
  Expr* acs;
  Expr* expr;
};

class Write : public Stmt {
 public:
  Write(Expr* e, int line) : Stmt(line), expr(e) {}
  ~Write() { delete expr; } 
  std::string to_string() { return "write: " + expr->to_string(); }
  Expr* expr;
};
// if, loop, assign, write, skip
// condition?
// array assignment
// sequence of statments

#endif

