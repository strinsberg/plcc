#ifndef PLCC_EXPRESSIONS_H
#define PLCC_EXPRESSIONS_H

#include "AstNode.h"
#include "Symbol.h"
#include "Tokens.h"
#include <string>


class Expr : public AstNode {
 public:
  Expr(Token* tok, yytokentype t, int line) : AstNode(line), op(tok), type(t) {}
  virtual ~Expr() { }
  virtual std::string to_string() { return op->to_string() + "(" + tok_string.at(type) + ")"; }

  Token* op;
  yytokentype type;
};

class Id : public Expr {
 public:
  Id(Word* w, yytokentype t, yytokentype k, int line) : Expr(w, t, line), kind(k) {}
  virtual ~Id() { delete op; }
  virtual std::string to_string() { return Expr::to_string() + "(" + tok_string.at(kind) + ")"; }
  yytokentype kind;
};

class Constant : public Expr {
 public:
  Constant(Token* tok, yytokentype t, int line) : Expr(tok, t, line) {}
  ~Constant() { delete op; }
};

class Access : public Expr {
 public:
  Access(Id* i, int line) : Expr(i->op, i->type, line), id(i) {}
  ~Access() { }
  virtual std::string to_string() { return Expr::to_string() + "(Access)"; }
  Id* id;
};

class ArrayAccess : public Access {
 public:
  ArrayAccess(Id* i, Expr* idx, int line) : Access(i, line), index(idx) {} 
  ~ArrayAccess() { delete index; }
  virtual std::string to_string() { return Access::to_string() + "[" + index->to_string() + "]"; }

  Expr* index;
};

class Binary : public Expr {
 public:
  Binary(Token* op, Expr* l, Expr* r, int line) : Expr(op, l->type, line), lhs(l), rhs(r) {}
  ~Binary() { delete lhs; delete rhs; }

  Expr* lhs;
  Expr* rhs;
};

class Unary : public Expr {
 public:
  Unary(Token* op, Expr* e, int line) : Expr(op, e->type, line), expr(e) {}
  ~Unary() { delete expr; }

  Expr* expr;
};
// possible classes needed:
// op, arith, unary
// temp -- for the temporary identifier you emit?
// relational
// logical
// array access

#endif
