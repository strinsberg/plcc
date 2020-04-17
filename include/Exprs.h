#ifndef PLCC_EXPRESSIONS_H
#define PLCC_EXPRESSIONS_H

#include "AstNode.h"
#include "CodeGen.h"
#include "Symbol.h"
#include "Tokens.h"
#include <string>


class Constant : public Expr {
 public:
  Constant(Type type, int value = 0, double dec = 0.0);
  virtual ~Constant();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 private:
  int value;
  double dec;
};


class Id : public Expr {
 public:
  Id(Word* word, Type type);
  Id(std::string lexeme, Type type);
  virtual ~Id();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  Word* word;
  std::string lexeme;
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
  Binary(Operator op, Expr* lhs, Expr* rhs);
  virtual ~Binary();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  Operator op;
  Expr* lhs;
  Expr* rhs;
};


class Unary : public Expr {
 public:
  Unary(Operator op, Expr* e);
  virtual ~Unary();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

 protected:
  Operator op;
  Expr* expr;
};

#endif
