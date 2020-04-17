#include "Defs.h"
#include "Exprs.h"
#include <iostream>
using namespace std;


// VARDEF /////////////////////////////////////////////////////////////
VarDef::VarDef(Id* i) : id(i) {}

VarDef::~VarDef() {
  delete id;
}

void VarDef::visit(CodeGen* generator) {
  generator->visit(this);
}

void VarDef::display(std::ostream& out) const {
  out << *id;
}


// DEFSEQ /////////////////////////////////////////////////////////////
DefSeq::DefSeq(Def* f, Def* r) : first(f), rest(r) {}

DefSeq::~DefSeq() {
  delete first;
  delete rest;
}

void DefSeq::visit(CodeGen* generator) {
  generator->visit(this);
}

void DefSeq::display(std::ostream& out) const {
  out << *first << endl << *rest;
}


// PROCDEF ////////////////////////////////////////////////////////////
ProcDef::ProcDef(Def* d, Stmt* s) : name(d), block(s)  {}

ProcDef::~ProcDef() {
  delete name;
  delete block;
}

void ProcDef::visit(CodeGen* generator) {
  generator->visit(this);
}

void ProcDef::display(std::ostream& out) const {
  out << endl << "PROC" << endl << *name;
  out << *block << "ENDPROC";
}

