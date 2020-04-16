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

std::ostream& operator<< (std::ostream& out, const Seq& node) {
  out << node.first << endl << node.rest;
  return out;
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

std::ostream& operator<< (std::ostream& out, const Block& node) {
  out << node.defs << endl << node.stmts;
  return out;
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

std::ostream& operator<< (std::ostream& out, const Asgn& node) {
  out << node.acs << " := " << node.expr << endl;
  return out;
}

// WRITE //////////////////////////////////////////////////////////////
Write::Write(Expr* e) : Stmt(), expr(e) {}

Write::~Write() {
  delete expr;
}

void Write::visit(CodeGen* generator) {
  generator->visit(this);
}

std::ostream& operator<< (std::ostream& out, const Write& node) {
  out << "Write: " << node.expr;
  return out;
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

std::ostream& operator<< (std::ostream& out, const Cond& node) {
  out << "cond:" << endl << node.cond << endl << node.stmts << endl << "ENDCOND";
  return out;
}


// LOOP ///////////////////////////////////////////////////////////////
Loop::Loop(Stmt* c) : Stmt(), cond(c) {}

Loop::~Loop() {
  delete cond;
}

void Loop::visit(CodeGen* generator) {
  generator->visit(this);
}

std::ostream& operator<< (std::ostream& out, const Loop& node) {
  out << "loop:" << endl << node.cond << endl << "ENDLOOP";
  return out;
}


// IF /////////////////////////////////////////////////////////////////
IfStmt::IfStmt(Stmt* c) : Stmt(), conds(c) {}

IfStmt::~IfStmt() {
  delete conds;
}

void IfStmt::visit(CodeGen* generator) {
  generator->visit(this);
}

std::ostream& operator<< (std::ostream& out, const IfStmt& node) {
  out << "if:" << endl << node.conds << endl << "ENDIF";
  return out;
}
