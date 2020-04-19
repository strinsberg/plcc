#ifndef PLCC_DEFS_H
#define PLCC_DEFS_H

#include "AstNode.h"
#include "Exprs.h"
#include <iostream>
#include <string>
#include <memory>


class VarDef : public Def {
 public:
  VarDef(std::shared_ptr<Id> i);
  virtual ~VarDef();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  std::shared_ptr<Id> id;
};


class DefSeq : public Def {
 public:
  DefSeq(std::shared_ptr<Def> first, std::shared_ptr<Def> rest);
  virtual ~DefSeq();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  std::shared_ptr<Def> first;
  std::shared_ptr<Def> rest;
};


class ProcDef : public Def {
 public:
  ProcDef(std::shared_ptr<Expr> name, std::shared_ptr<Stmt> block);
  virtual ~ProcDef();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  std::shared_ptr<Expr> name;
  std::shared_ptr<Stmt> block;
};

#endif
