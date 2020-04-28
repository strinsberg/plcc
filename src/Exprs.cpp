#include "Exprs.h"
#include "Types.h"
#include "TreeWalker.h"
#include "Symbol.h"
#include "exceptions.h"
#include <iostream>
#include <memory>
using namespace std;


// EXPR ///////////////////////////////////////////////////////////////

Expr::Expr(Type t) : AstNode("ExprNode"), type(t) {}

Expr::~Expr() {}

void Expr::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Expr::display(ostream& out) const {
  out << name << "(";
  if (type.type == symbol::RECORD)
    out << type.name;
  else
    out << symbol::to_string.at(type.type);
  out << ")";
}


// Access /////////////////////////////////////////////////////////////

Access::Access(string n, int s, shared_ptr<Type> t)
    : Expr(t), name(n), size(s) {} 

  // This works as long as it is not possible to have const arrays
  // Should this be kept????
  if (type.kind == symbol::ARRAY)
    type.qual = symbol::ARRAY;  // We are accessing the whole array
}

Access::~Access() {}

void Access::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Access::display(ostream& out) const {
  Expr::display(out);
  out << "(access)";
}


// ArrayAccess ////////////////////////////////////////////////////////

ArrayAccess::ArrayAccess(string n, shared_ptr<Type> t, shared_ptr<Expr> idx)
    : Access(i, 1, t), index(idx) {

  // If arrays can ever hold arrays or records then the size will
  // need to be set appropriately

  if (type.kind != symbol::ARRAY)
    throw type_error("variable must be array");

  if (index->get_type().type != symbol::INT)
    throw type_error("array index must be type int");

  type.qual = symbol::SCALAR;  // we are accessing only one value
}

ArrayAccess::~ArrayAccess() {}

void ArrayAccess::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void ArrayAccess::display(ostream& out) const {
  out << *id << "[" << *index << "]";
}


// Record Access //////////////////////////////////////////////////////

RecAccess::RecAccess(string n, shared_ptr<Type> t) : Access(n, t) {}
  if (type.kind != symbol::RECORD) {
    throw type_error("variable is not a record");
  }
}

RecAccess::~RecAccess() {}

void RecAccess::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void RecAccess::display(std::ostream& out) const {
  out << *record << " DOT " << *field;
}

void RecAccess::add_access(std::shared_ptr<Access> access) {
  if (acs.size() and acs.back().get_type().kind != symbol::RECORD)
    throw type_error("variable is not a record");

  acs.push_back(access);
}


// Binary /////////////////////////////////////////////////////////////

Binary::Binary(Operator o, shared_ptr<Expr> l, shared_ptr<Expr> r)
    : Expr(l->get_type()), op(o), lhs(l), rhs(r) {

  if (o.type.qual != symbol::UNIVERSAL) {
    type.type = o.type.qual;
    type.kind = symbol::UNIVERSAL;
  }

  if (!(lhs->get_type().type == rhs->get_type().type))
    throw type_error("type mismatch for binary operator");

  if (!op.accepts(lhs) or !op.accepts(rhs))
    throw type_error("invalid types for binary operator");
}

Binary::~Binary() {}

void Binary::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Binary::display(ostream& out) const {
  out << "{ " << *lhs << " ## " << symbol::str(op.op) << " ## " << *rhs << " }";
}


// Unary //////////////////////////////////////////////////////////////

Unary::Unary(Operator o, shared_ptr<Expr> e)
    : Expr(e->get_type()), op(o), expr(e) {
  if (!op.accepts(e))
    throw type_error("invalid type for unary operator");
}

Unary::~Unary() {}

void Unary::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Unary::display(ostream& out) const {
  out << "{ " << symbol::str(op.op) << " # " << *expr << " }";
}

