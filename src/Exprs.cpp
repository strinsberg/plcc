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
Access::Access(Id* i) : Expr(), id(i) {}

Access::~Access() {
  delete id;
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
  delete index;
}

void ArrayAccess::visit(CodeGen* generator) {
  generator->visit(this);
}

void ArrayAccess::display(ostream& out) const {
  out << *id << "[" << *index << "]";
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

void Binary::display(ostream& out) const {
  out << *lhs << " ## " << token->to_string() << " ## " << *rhs;
}


// Unary //////////////////////////////////////////////////////////////
Unary::Unary(Token* op, Expr* e) : Expr(op, e->get_type()), expr(e) {}

Unary::~Unary() {
  delete expr;
}

void Unary::visit(CodeGen* generator) {
  generator->visit(this);
}

void Unary::display(ostream& out) const {
  out << token->to_string() << " ++ " << *expr;
}

