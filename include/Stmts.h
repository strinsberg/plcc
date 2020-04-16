#ifndef PLCC_STATEMENTS_H
#define PLCC_STATEMENTS_H

#include "AstNode.h"
#include "CodeGen.h"
#include <string>


class Seq : public Stmt {
 public:
  Seq(Stmt* first, Stmt* rest);
  virtual ~Seq();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& out) const;

 protected:
  Stmt* first;
  Stmt* rest;
};


class Block : public Stmt {
 public:
  Block(Def* defs, Stmt* stmts);
  virtual ~Block();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& out) const;

 protected:
  Def* defs;
  Stmt* stmts;
};


class Asgn : public Stmt {
 public:
  Asgn(Expr* access, Expr* expr);
  virtual ~Asgn();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& out) const;

 protected:
  Expr* acs;
  Expr* expr;
};


class Write : public Stmt {
 public:
  Write(Expr* expr);
  virtual ~Write();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& out) const;

 protected:
  Expr* expr;
};


class Cond : public Stmt {
 public:
  Cond(Expr* condition, Stmt* stmts);
  virtual ~Cond();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& out) const;

 protected:
  Expr* cond;
  Stmt* stmts;
};


class Loop : public Stmt {
 public:
  Loop(Stmt* condition);
  virtual ~Loop();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& out) const;

 protected:
  Stmt* cond;
};


class IfStmt : public Stmt {
 public:
  IfStmt(Stmt* conditions);
  virtual ~IfStmt();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& out) const;

 protected:
  Stmt* conds;
};

#endif

