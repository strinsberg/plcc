#ifndef PLCC_DEFS_H
#define PLCC_DEFS_H

#include "AstNode.h"
#include "Exprs.h"
#include <iostream>
#include <string>


class VarDef : public Def {
 public:
  VarDef(Id* i);
  virtual ~VarDef();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  Id* id;
};


class DefSeq : public Def {
 public:
  DefSeq(Def* first, Def* rest);
  virtual ~DefSeq();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  Def* first;
  Def* rest;
};


class ProcDef : public Def {
 public:
  ProcDef(Expr* name, Stmt* block);
  virtual ~ProcDef();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  Expr* name;
  Stmt* block;
};

#endif
