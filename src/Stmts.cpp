#include "Stmts.h"
#include "AstNode.h"
#include "Exprs.h"
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
Asgn::Asgn(Expr* a, Expr* e) : Stmt(), acs(a), expr(e) {}

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
Write::Write(Expr* e) : Stmt(), expr(e) {}

Write::~Write() {
  delete expr;
}

void Write::visit(CodeGen* generator) {
  generator->visit(this);
}

void Write::display(ostream& out) const {
  out << "WRITE: " << *expr;
}


// COND ///////////////////////////////////////////////////////////////
Cond::Cond(Expr* c, Stmt* s) : Stmt(), cond(c), stmts(s) {}

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
Proc::Proc(Id* i) : Stmt(), id(i) {}

Proc::~Proc() {
  // The id is owned by a def node. Do Not Delete it!
}

void Proc::visit(CodeGen* generator) {
  generator->visit(this);
}

void Proc::display(std::ostream& out) const {
  out << "Call: " << *id;
}
