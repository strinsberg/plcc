#ifndef PLCC_EXPRESSIONS_H
#define PLCC_EXPRESSIONS_H

#include "AstNode.h"
#include "CodeGen.h"
#include "Symbol.h"
#include "Tokens.h"
#include <string>


class Constant : public Expr {
 public:
  Constant(Token* token, symbol::Tag type);
  virtual ~Constant();
  virtual void visit(CodeGen* generator);
};


class Id : public Expr {
 public:
  Id(Word* word, symbol::Tag type, symbol::Tag kind);
  virtual ~Id();
  virtual void visit(CodeGen* generator);

 protected:
  symbol::Tag kind;
};


class Access : public Expr {
 public:
  Access(Id* id);
  virtual ~Access();
  virtual void visit(CodeGen* generator);

 protected:
  Id* id;
};


class ArrayAccess : public Access {
 public:
  ArrayAccess(Id* id, Expr* index);
  virtual ~ArrayAccess();
  virtual void visit(CodeGen* generator);

 protected:
  Expr* index;
};


class Binary : public Expr {
 public:
  Binary(Token* op, Expr* lhs, Expr* rhs);
  virtual ~Binary();
  virtual void visit(CodeGen* generator);

 protected:
  Expr* lhs;
  Expr* rhs;
};


class Unary : public Expr {
 public:
  Unary(Token* op, Expr* e);
  virtual ~Unary();
  virtual void visit(CodeGen* generator);

 protected:
  Expr* expr;
};

#endif
