#include "Stmts.h"
#include "AstNode.h"
#include "Exprs.h"
#include "TreeWalker.h"
#include "exceptions.h"
#include <iostream>
#include <memory>
using namespace std;


// SEQ ////////////////////////////////////////////////////////////////

Seq::Seq(shared_ptr<Stmt> f, shared_ptr<Stmt> r)
    : Stmt(), first(f), rest(r) {}

Seq::~Seq() {}

void Seq::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Seq::display(ostream& out) const {
  out << *first << endl << *rest;
}

// Block Stmt /////////////////////////////////////////////////////////

BlockStmt::BlockStmt(std::shared_ptr<Stmt> b) : block(b) {}

BlockStmt::~BlockStmt() {}

void BlockStmt::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void BlockStmt::display(std::ostream& out) const {
  out << endl << "BLOCK STMT";
  out << *block;
}


// Block //////////////////////////////////////////////////////////////

Block::Block(shared_ptr<Def> d, shared_ptr<Stmt> s)
    : Stmt(), defs(d), stmts(s) {}

Block::~Block() {}

void Block::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Block::display(ostream& out) const {
  out << endl << "BLOCK" << endl;
  out << "==DEFS==" << endl << *defs << endl << endl;
  out << "==STMTS==" << endl << *stmts << endl;
  out << "ENDBLOCK" << endl;
}


// ASGN ///////////////////////////////////////////////////////////////

Asgn::Asgn(shared_ptr<Expr> a, shared_ptr<Expr> e) : Stmt(), acs(a), expr(e) {

  if (!(acs->get_type().type == expr->get_type().type))
    throw type_error("assignment variable type does not match expression type");

  if (acs->get_type().qual == symbol::CONST)
    throw type_error("cannot assign to a constant");
}

Asgn::~Asgn() {}

void Asgn::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Asgn::display(ostream& out) const {
  out << *acs << " := " << *expr;
}


// String Assignment //////////////////////////////////////////////////

StringAsgn::StringAsgn(shared_ptr<Expr> a, shared_ptr<Expr> e)
    : Stmt(), acs(a), str(e) {

  Type acs_type = acs->get_type();
  Type str_type = str->get_type();
  if (acs_type.type != symbol::CHAR or acs_type.kind != symbol::ARRAY
      or str_type.type != symbol::STRING or str_type.kind != symbol::ARRAY)
    throw type_error("string assignment arguments must be char array and string literal");

  int size = acs->get_size();
  int length = str->get_size(); 
  if (length + 1 > size)
    throw type_error("char array must be at least 1 longer than string literal");
}

StringAsgn::~StringAsgn() {}

void StringAsgn::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void StringAsgn::display(std::ostream& out) const {
  out << *acs << " := " << *str;
}

// WRITE/READ /////////////////////////////////////////////////////////

IoStmt::IoStmt(shared_ptr<Expr> e, symbol::Tag t)
    : Stmt(), expr(e), type(t) {
  if (type == symbol::READ and expr->get_type().qual == symbol::ARRAY
      and expr->get_type().type != symbol::CHAR)
    throw type_error("can only read into char array without index");
}

IoStmt::~IoStmt() {}

void IoStmt::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void IoStmt::display(ostream& out) const {
  out << symbol::str(type) << ": " << *expr;
}


// ReadLine ///////////////////////////////////////////////////////////

ReadLine::ReadLine(std::shared_ptr<Expr> id) : Stmt(), array_id(id) {
  if (array_id->get_type().type != symbol::CHAR
      or array_id->get_type().kind != symbol::ARRAY)
    throw type_error("readline only works with character arrays");
}

ReadLine::~ReadLine() {}

void ReadLine::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void ReadLine::display(std::ostream& out) const {
  out << "Readline: " << *array_id;
}


// SEQ ////////////////////////////////////////////////////////////////

CondSeq::CondSeq(shared_ptr<Stmt> f, shared_ptr<Stmt> r) : Seq(f, r) {}

CondSeq::~CondSeq() {}

void CondSeq::visit(TreeWalker& walker) {
  walker.visit(*this);
}


// COND ///////////////////////////////////////////////////////////////

Cond::Cond(shared_ptr<Expr> c, shared_ptr<Stmt> s)
    : Stmt(), cond(c), stmts(s) {
  if (cond->get_type().type != symbol::BOOL)
    throw type_error("condition must be boolean expression");
}

Cond::~Cond() {}

void Cond::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Cond::display(ostream& out) const {
  out << "++COND++" << endl << *cond << endl;
  out << *stmts << endl << "++ENDCOND++";
}


// LOOP ///////////////////////////////////////////////////////////////

Loop::Loop(shared_ptr<Stmt> c) : Stmt(), cond(c) {}

Loop::~Loop() {}

void Loop::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Loop::display(ostream& out) const {
  out << "**LOOP**" << endl << *cond << endl << "**ENDLOOP**";
}


// IF /////////////////////////////////////////////////////////////////

IfStmt::IfStmt(shared_ptr<Stmt> c) : Stmt(), conds(c) {}

IfStmt::~IfStmt() {}

void IfStmt::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void IfStmt::display(ostream& out) const {
  out << "--IF--" << endl << *conds << endl << "--ENDIF--";
}


// PROC ///////////////////////////////////////////////////////////////

Proc::Proc(shared_ptr<Id> i) : Stmt(), id(i) {
  if (i->get_type().kind != symbol::PROC)
    throw type_error("variable is not a procedure");
}

Proc::~Proc() {
  // The id is owned by a def node. Do Not Delete it!
}

void Proc::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Proc::display(ostream& out) const {
  out << "Call: " << *id;
}
