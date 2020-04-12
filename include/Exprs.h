#ifndef PLCC_EXPRESSIONS_H
#define PLCC_EXPRESSIONS_H

#include "AstNode.h"
#include "Symbol.h"
#include <string>


class Expr : public AstNode {
 public:
  Expr(Token* tok, yytokentype t, int line) : AstNode(line), op(tok), type(t) {}
  virtual ~Expr() { delete op; }
  virtual std::string to_string() { return std::to_string(line_num) + ": " + tok_string.at(type) + " -> " + op->to_string(); }

  Token* op;
  yytokentype type;
};

class Id : public Expr {
 public:
  Id(Word* w, yytokentype t, int line) : Expr(w, t, line) {}
  virtual ~Id() {}
};


// classes needed:
// op, arith, unary
// temp -- for the temporary identifier you emit?
// relational
// logical
// array access

#endif
