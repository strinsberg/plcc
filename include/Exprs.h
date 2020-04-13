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
  virtual std::string to_string() { return op->to_string() + "(" + tok_string.at(type) + "," + tok_string.at(kind) + ")"; }
  yytokentype kind;
};

class Constant : public Expr {
 public:
  Constant(Token* tok, yytokentype t, int line) : Expr(tok, t, line) {}
  ~Constant() { delete op; }
};

class Access : public Expr {
 public:
  Access(Id* i, Expr* e, int line) : Expr(i->op, i->type, line), id(i), expr(e) {}
  ~Access() { if (expr != nullptr) delete expr; }
  Id* id;
  Expr* expr;
};

// possible classes needed:
// op, arith, unary
// temp -- for the temporary identifier you emit?
// relational
// logical
// array access

#endif
