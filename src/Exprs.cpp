#include "Exprs.h"
#include "Tokens.h"
#include "Symbol.h"
#include <iostream>
using namespace std;


// Constant ///////////////////////////////////////////////////////////
Constant::Constant(Token* token, symbol::Tag type) : Expr(token, type) {}

Constant::~Constant() {}

void Constant::visit(CodeGen* generator) {
  generator->visit(this);
}

std::ostream& operator<< (std::ostream& out, const Constant& node) {
  out << node;  // Use the Parent's?
  return out;
}


// Id /////////////////////////////////////////////////////////////////
Id::Id(Word* word, symbol::Tag type, symbol::Tag k)
    : Expr(word, type), kind(k) {}

Id::~Id() {}

void Id::visit(CodeGen* generator) {
  generator->visit(this);
}

std::ostream& operator<< (std::ostream& out, const Id& node) {
  out << node << "(" << symbol::to_string.at(node.kind) << ")";
  return out;
}


// Access /////////////////////////////////////////////////////////////
Access::Access(Id* i) : Expr(), id(i) {}

Access::~Access() {
  delete id;
}

void Access::visit(CodeGen* generator) {
  generator->visit(this);
}

std::ostream& operator<< (std::ostream& out, const Access& node) {
  out << node << "(access)";
  return out;
}


// ArrayAccess ////////////////////////////////////////////////////////
ArrayAccess::ArrayAccess(Id* i, Expr* idx) : Access(i), index(idx) {}

ArrayAccess::~ArrayAccess() {
  delete index;
}

void ArrayAccess::visit(CodeGen* generator) {
  generator->visit(this);
}

std::ostream& operator<< (std::ostream& out, const ArrayAccess& node) {
  out << node << "[" << node.index << "]";
  return out;
}


// Binary /////////////////////////////////////////////////////////////
Binary::Binary(Token* op, Expr* l, Expr* r)
    : Expr(op, l->get_type()), lhs(l), rhs(r) {}

Binary::~Binary() {
  delete lhs;
  delete rhs;
}

void Binary::visit(CodeGen* generator) {
  generator->visit(this);
}

std::ostream& operator<< (std::ostream& out, const Binary& node) {
  out << node;
  return out;
}


// Unary //////////////////////////////////////////////////////////////
Unary::Unary(Token* op, Expr* e) : Expr(op, e->get_type()), expr(e) {}

Unary::~Unary() {
  delete expr;
}

void Unary::visit(CodeGen* generator) {
  generator->visit(this);
}

std::ostream& operator<< (std::ostream& out, const Unary& node) {
  out << node;
  return out;
}

