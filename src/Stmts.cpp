#include "Stmts.h"
#include "AstNode.h"
#include "Exprs.h"
#include "exceptions.h"
#include <iostream>
#include <memory>
using namespace std;


// SEQ ////////////////////////////////////////////////////////////////
Seq::Seq(std::shared_ptr<Stmt> f, std::shared_ptr<Stmt> r) : Stmt(), first(f), rest(r) {}

Seq::~Seq() {}

void Seq::visit(CodeGen* generator) {
  generator->visit(this);
}

void Seq::display(ostream& out) const {
  out << *first << endl << *rest;
}


// Block //////////////////////////////////////////////////////////////
Block::Block(std::shared_ptr<Def> d, std::shared_ptr<Stmt> s) : Stmt(), defs(d), stmts(s) {}

Block::~Block() {}

void Block::visit(CodeGen* generator) {
  generator->visit(this);
}

void Block::display(ostream& out) const {
  out << endl << "BLOCK" << endl;
  out << "==DEFS==" << endl << *defs << endl << endl;
  out << "==STMTS==" << endl << *stmts << endl;
  out << "ENDBLOCK" << endl;
}


// ASGN ///////////////////////////////////////////////////////////////
Asgn::Asgn(std::shared_ptr<Expr> a, std::shared_ptr<Expr> e) : Stmt(), acs(a), expr(e) {
  if (!(acs->get_type().type == expr->get_type().type))
    throw type_error("assignment variable type does not match expression type");

  if (acs->get_type().qual == symbol::CONST)
    throw type_error("cannot assign to a constant");
}

Asgn::~Asgn() {}

void Asgn::visit(CodeGen* generator) {
  generator->visit(this);
}

void Asgn::display(ostream& out) const {
  out << *acs << " := " << *expr;
}

// WRITE //////////////////////////////////////////////////////////////
IoStmt::IoStmt(std::shared_ptr<Expr> e, symbol::Tag t) : Stmt(), expr(e), type(t) {}

IoStmt::~IoStmt() {}

void IoStmt::visit(CodeGen* generator) {
  generator->visit(this);
}

void IoStmt::display(ostream& out) const {
  out << symbol::str(type) << ": " << *expr;
}


// COND ///////////////////////////////////////////////////////////////
Cond::Cond(std::shared_ptr<Expr> c, std::shared_ptr<Stmt> s) : Stmt(), cond(c), stmts(s) {
  if (cond->get_type().type != symbol::BOOL)
    throw type_error("condition must be boolean expression");
}

Cond::~Cond() {}

void Cond::visit(CodeGen* generator) {
  generator->visit(this);
}

void Cond::display(ostream& out) const {
  out << "++COND++" << endl << *cond << endl;
  out << *stmts << endl << "++ENDCOND++";
}


// LOOP ///////////////////////////////////////////////////////////////
Loop::Loop(std::shared_ptr<Stmt> c) : Stmt(), cond(c) {}

Loop::~Loop() {}

void Loop::visit(CodeGen* generator) {
  generator->visit(this);
}

void Loop::display(ostream& out) const {
  out << "**LOOP**" << endl << *cond << endl << "**ENDLOOP**";
}


// IF /////////////////////////////////////////////////////////////////
IfStmt::IfStmt(std::shared_ptr<Stmt> c) : Stmt(), conds(c) {}

IfStmt::~IfStmt() {}

void IfStmt::visit(CodeGen* generator) {
  generator->visit(this);
}

void IfStmt::display(ostream& out) const {
  out << "--IF--" << endl << *conds << endl << "--ENDIF--";
}


// PROC ///////////////////////////////////////////////////////////////
Proc::Proc(std::shared_ptr<Id> i) : Stmt(), id(i) {
  if (i->get_type().kind != symbol::PROC)
    throw type_error("variable is not a procedure");
}

Proc::~Proc() {
  // The id is owned by a def node. Do Not Delete it!
}

void Proc::visit(CodeGen* generator) {
  generator->visit(this);
}

void Proc::display(std::ostream& out) const {
  out << "Call: " << *id;
}
