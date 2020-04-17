#include "Exprs.h"
#include "Types.h"
#include "Symbol.h"
#include "exceptions.h"
#include <iostream>
using namespace std;


// Constant ///////////////////////////////////////////////////////////
Constant::Constant(Type t, int v, double d)
  : Expr(t), value(v), dec(d) {}

Constant::~Constant() {}

void Constant::visit(CodeGen* generator) {
  generator->visit(this);
}

void Constant::display(ostream& out) const {
  if (type.type == symbol::FLOAT)
    out << dec;
  else if (type.type == symbol::CHAR)
    out << (char)value;
  else
    out << value;
  Expr::display(out);
}


// Id /////////////////////////////////////////////////////////////////

Id::Id(string l, Type type) : Expr(type) {
  name = l;
}

Id::~Id() {}

void Id::visit(CodeGen* generator) {
  generator->visit(this);
}

void Id::display(ostream& out) const {
  out << name;
  Expr::display(out);
  out << "(" << symbol::str(type.kind) << ")";
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
Binary::Binary(Operator o, Expr* l, Expr* r)
    : Expr(l->get_type()), op(o), lhs(l), rhs(r) {
  if (!(lhs->get_type() == rhs->get_type()))
    throw type_error("type mismatch for binary operator");

  if (!op.accepts(lhs) or !op.accepts(rhs))
    throw type_error("invalid types for binary operator");
}

Binary::~Binary() {
  delete lhs;
  delete rhs;
}

void Binary::visit(CodeGen* generator) {
  generator->visit(this);
}

void Binary::display(ostream& out) const {
  out << *lhs << " ## " << symbol::str(op.op) << " ## " << *rhs;
}


// Unary //////////////////////////////////////////////////////////////
Unary::Unary(Operator o, Expr* e) : Expr(e->get_type()), op(o), expr(e) {
  if (!op.accepts(e))
    throw type_error("invalid type for unary operator");
}

Unary::~Unary() {
  delete expr;
}

void Unary::visit(CodeGen* generator) {
  generator->visit(this);
}

void Unary::display(ostream& out) const {
  out << symbol::str(op.op) << " # " << *expr;
}

