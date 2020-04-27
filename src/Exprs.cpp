#include "Exprs.h"
#include "Types.h"
#include "TreeWalker.h"
#include "Symbol.h"
#include "exceptions.h"
#include <iostream>
#include <memory>
using namespace std;


// Constant ///////////////////////////////////////////////////////////

Constant::Constant(int v)
    : Expr( Type(symbol::INT, symbol::UNIVERSAL, symbol::CONST) ),
      value(v), exp(1) {}

Constant::Constant(Type t, int v, int e)
    : Expr(t), value(v), exp(e) {}

Constant::~Constant() {}

void Constant::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Constant::display(ostream& out) const {
  if (type.type == symbol::CHAR) {
    out << (char)value;
  } else if (type.type == symbol::FLOAT) {
    out << (double)value / exp;

  } else {
    out << value;
  }
  Expr::display(out);
}


// Constant String ////////////////////////////////////////////////////

ConstString::ConstString(std::string str) : Constant(str.size() - 2),
    text(str.substr(1, str.size() - 2)) {
  type.type = symbol::STRING;
  type.kind = symbol::ARRAY;
}

ConstString::~ConstString() {}

void ConstString::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void ConstString::display(std::ostream& out) const {
  out << "\"" << text << "\"";
}


// Id /////////////////////////////////////////////////////////////////

Id::Id(string l, Type type, shared_ptr<Expr> s) : Expr(type), size(s) {
  name = l;

  // array ids must have a const int size
  if (type.kind == symbol::ARRAY) {
    if (size->get_type().qual != symbol::CONST)
      throw type_error("array size must be a constant");

    if (size->get_type().type != symbol::INT)
      throw type_error("array size must be int");
  }
}

Id::~Id() {}

void Id::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Id::display(ostream& out) const {
  out << name;
  Expr::display(out);
  out << "(" << symbol::str(type.kind) << ")";
}


// Const Id ////////////////////////////////////////////////////////////

ConstId::ConstId(string l, Type t, shared_ptr<Expr> c) 
    : Id(l, t, make_shared<Constant>()), value(c) {

  if (t.type != value->get_type().type)
    throw type_error("constant variable type does not match value type");

  if (c->get_type().qual != symbol::CONST)
    throw type_error("values assigned to constants must be constant");
}

ConstId::~ConstId() {}

void ConstId::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void ConstId::display(ostream& out) const {
  Id::display(out);
  out << "(Const: " << *value << ")";
}


// Access /////////////////////////////////////////////////////////////

Access::Access(shared_ptr<Id> i) : Expr( i->get_type() ) , id(i) {
  name = id->get_name();

  // This works as long as it is not possible to have const arrays
  if (type.kind == symbol::ARRAY)
    type.qual = symbol::ARRAY;  // We are accessing the whole array
}

Access::~Access() {}

void Access::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Access::display(ostream& out) const {
  out << *id << "(access)";
}


// ArrayAccess ////////////////////////////////////////////////////////

ArrayAccess::ArrayAccess(shared_ptr<Id> i, shared_ptr<Expr> idx)
    : Access(i), index(idx) {
  if (i->get_type().kind != symbol::ARRAY)
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

RecAccess::RecAccess(shared_ptr<Expr> r, shared_ptr<Expr> f)
    : Expr(f->get_type()), record(r), field(f) {
  name = record->get_name() + "." + field->get_name();
  //if (record->get_type().kind != symbol::RECORD)
    //throw type_error("variable is not a record");
}

RecAccess::~RecAccess() {}

void RecAccess::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void RecAccess::display(std::ostream& out) const {
  out << *record << " DOT " << *field;
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

