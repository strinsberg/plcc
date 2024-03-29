#include "Types.h"
#include "Symbol.h"
#include "AstNode.h"
#include <iostream>
#include <string>
#include <memory>
using namespace std;


// Type ///////////////////////////////////////////////////////////////

Type::Type() : type(symbol::EMPTY), kind(symbol::UNIVERSAL),
    qual(symbol::UNIVERSAL) {}


Type::Type(symbol::Tag t, symbol::Tag k, symbol::Tag q, string n) 
    : type(t), kind(k), qual(q), name(n) {}


// might be obsolete and never used
bool Type::operator==(const Type& other) {
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


// Operator ///////////////////////////////////////////////////////////

bool Operator::accepts(shared_ptr<Expr> expr) {
  symbol::Tag t = expr->get_type().type;

  if (type.type == symbol::UNIVERSAL)
    return true;

  if (type.type != t) {
    if (type.type == symbol::NUMBER and (t == symbol::INT or t == symbol::FLOAT))
      return true;
    return false;
  }

  return true;
}
