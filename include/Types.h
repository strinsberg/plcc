#ifndef PLCC_TOKENS_H
#define PLCC_TOKENS_H

#include "Symbol.h"
#include <string>
#include <memory>
class Expr;


struct Type {
  Type();
  Type(symbol::Tag t, symbol::Tag k, symbol::Tag q);
  bool operator==(const Type& other);

  symbol::Tag type, kind, qual;
};


struct Operator {
  Operator() : op(symbol::EMPTY) {}
  Operator(symbol::Tag o, Type t) : op(o), type(t) {}
  bool accepts(std::shared_ptr<Expr> expr);

  symbol::Tag op;
  Type type;
};

#endif

