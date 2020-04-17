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
  Constant(Type type, int value = 0, double dec = 0.0);
  virtual ~Constant();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 private:
  Type type;
  int value;
  double dec;
};


class Id : public Expr {
 public:
  Id(Word* word, symbol::Tag type, symbol::Tag kind);
  virtual ~Id();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  symbol::Tag kind;
};


class Access : public Expr {
 public:
  Access(Id* id);
  virtual ~Access();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  Id* id;
};


class ArrayAccess : public Access {
 public:
  ArrayAccess(Id* id, Expr* index);
  virtual ~ArrayAccess();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  Expr* index;
};


class Binary : public Expr {
 public:
  Binary(Token* op, Expr* lhs, Expr* rhs);
  virtual ~Binary();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  Expr* lhs;
  Expr* rhs;
};


class Unary : public Expr {
 public:
  Unary(Token* op, Expr* e);
  virtual ~Unary();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  Expr* expr;
};

#endif
