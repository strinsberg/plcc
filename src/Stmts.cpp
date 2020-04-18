#include "Stmts.h"
#include "AstNode.h"
#include "Exprs.h"
#include "exceptions.h"
#include <iostream>
using namespace std;


// SEQ ////////////////////////////////////////////////////////////////
Seq::Seq(Stmt* f, Stmt* r) : Stmt(), first(f), rest(r) {}

Seq::~Seq() {
  delete first;
  delete rest;
}

void Seq::visit(CodeGen* generator) {
  generator->visit(this);
}

void Seq::display(ostream& out) const {
  out << *first << endl << *rest;
}


// Block //////////////////////////////////////////////////////////////
Block::Block(Def* d, Stmt* s) : Stmt(), defs(d), stmts(s) {}

Block::~Block() {
  delete defs;
  delete stmts;
}

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
Asgn::Asgn(Expr* a, Expr* e) : Stmt(), acs(a), expr(e) {
  if (!(acs->get_type().type == expr->get_type().type))
    throw type_error("assignment variable type does not match expression type");

  if (acs->get_type().qual == symbol::CONST)
    throw type_error("cannot assign to a constant");
}

Asgn::~Asgn() {
  delete acs;
  delete expr;
}

void Asgn::visit(CodeGen* generator) {
  generator->visit(this);
}

void Asgn::display(ostream& out) const {
  out << *acs << " := " << *expr;
}

// WRITE //////////////////////////////////////////////////////////////
IoStmt::IoStmt(Expr* e, symbol::Tag t) : Stmt(), expr(e), type(t) {}

IoStmt::~IoStmt() {
  delete expr;
}

void IoStmt::visit(CodeGen* generator) {
  generator->visit(this);
}

void IoStmt::display(ostream& out) const {
  out << symbol::str(type) << ": " << *expr;
}


// COND ///////////////////////////////////////////////////////////////
Cond::Cond(Expr* c, Stmt* s) : Stmt(), cond(c), stmts(s) {
  if (cond->get_type().type != symbol::BOOL)
    throw type_error("condition must be boolean expression");
}

Cond::~Cond() {
  delete cond;
  delete stmts;
}

void Cond::visit(CodeGen* generator) {
  generator->visit(this);
}

void Cond::display(ostream& out) const {
  out << "++COND++" << endl << *cond << endl;
  out << *stmts << endl << "++ENDCOND++";
}


// LOOP ///////////////////////////////////////////////////////////////
Loop::Loop(Stmt* c) : Stmt(), cond(c) {}

Loop::~Loop() {
  delete cond;
}

void Loop::visit(CodeGen* generator) {
  generator->visit(this);
}

void Loop::display(ostream& out) const {
  out << "**LOOP**" << endl << *cond << endl << "**ENDLOOP**";
}


// IF /////////////////////////////////////////////////////////////////
IfStmt::IfStmt(Stmt* c) : Stmt(), conds(c) {}

IfStmt::~IfStmt() {
  delete conds;
}

void IfStmt::visit(CodeGen* generator) {
  generator->visit(this);
}

void IfStmt::display(ostream& out) const {
  out << "--IF--" << endl << *conds << endl << "--ENDIF--";
}


// PROC ///////////////////////////////////////////////////////////////
Proc::Proc(Id* i) : Stmt(), id(i) {
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
