#ifndef PLCC_TOKENS_H
#define PLCC_TOKENS_H

#include "Symbol.h"
#include <string>


struct Type {
  Type() : type(symbol::EMPTY), kind(symbol::UNIVERSAL), qual(symbol::UNIVERSAL) {}
  Type(symbol::Tag t, symbol::Tag k, symbol::Tag q)
      : type(t), kind(k), qual(q) {}

  // This will need to be more complicated or updated in Op to group some types
  // for certain operators. ie kind == numeric or bool etc
  bool operator==(const Type& other) {
    if (type != other.type)
      return false;
    if (kind != other.kind and kind != symbol::UNIVERSAL
        and other.kind != symbol::UNIVERSAL)
      return false;
    if (qual != other.qual and qual != symbol::UNIVERSAL
        and other.qual != symbol::UNIVERSAL)
      return false;
    return true;
  }

  symbol::Tag type, kind, qual;
};

struct Operator {
  Operator() : op(symbol::EMPTY) {}
  Operator(symbol::Tag o, Type t) : op(o), type(t) {}
  symbol::Tag op;
  Type type;
};

#endif

