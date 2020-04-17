#include "Exprs.h"
#include "Tokens.h"
#include "Symbol.h"
#include "exceptions.h"
#include <iostream>
using namespace std;


// Constant ///////////////////////////////////////////////////////////
Constant::Constant(Token* token, symbol::Tag type) : Expr(token, type) {}

Constant::~Constant() {}

void Constant::visit(CodeGen* generator) {
  generator->visit(this);
}

void Constant::display(ostream& out) const {
  Expr::display(out);
}


// Id /////////////////////////////////////////////////////////////////
Id::Id(Word* word, symbol::Tag type, symbol::Tag k)
    : Expr(word, type), kind(k) {}

Id::~Id() {}

void Id::visit(CodeGen* generator) {
  generator->visit(this);
}

void Id::display(ostream& out) const {
  Expr::display(out);
  out << "(" << symbol::to_string.at(kind) << ")";
}


// Access /////////////////////////////////////////////////////////////
Access::Access(Id* i) : Expr( i->get_type() ) , id(i) {}

Access::~Access() {
  // A definition owns the id so do not delete it!
}

void Access::visit(CodeGen* generator) {
  generator->visit(this);
}

void Access::display(ostream& out) const {
  out << *id << "(access)";
}


// ArrayAccess ////////////////////////////////////////////////////////
ArrayAccess::ArrayAccess(Id* i, Expr* idx) : Access(i), index(idx) {}

ArrayAccess::~ArrayAccess() {
  // do not delete the index if it is an id
  //delete index;
}

void ArrayAccess::visit(CodeGen* generator) {
  generator->visit(this);
}

void ArrayAccess::display(ostream& out) const {
  out << *id << "[" << *index << "]";
}


// Binary /////////////////////////////////////////////////////////////
Binary::Binary(Token* op, Expr* l, Expr* r)
    : Expr(op, l->get_type()), lhs(l), rhs(r) {
  if (lhs->get_type() != rhs->get_type())
    throw type_error("type mismatch for binary operator");
  // can also add a check to make sure type of the operator is
  // valid for the types of the expressions. Ie < need 2 bool.
}

Binary::~Binary() {
  delete lhs;
  delete rhs;
}

void Binary::visit(CodeGen* generator) {
  generator->visit(this);
}

void Binary::display(ostream& out) const {
  out << *lhs << " ## " << token->to_string() << " ## " << *rhs;
}


// Unary //////////////////////////////////////////////////////////////
Unary::Unary(Token* op, Expr* e) : Expr(op, e->get_type()), expr(e) {
  // check that type of operator matches expr
}

Unary::~Unary() {
  delete expr;
}

void Unary::visit(CodeGen* generator) {
  generator->visit(this);
}

void Unary::display(ostream& out) const {
  out << token->to_string() << " ++ " << *expr;
}

