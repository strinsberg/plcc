#include "Defs.h"
#include "Exprs.h"
#include <iostream>
#include <memory>
using namespace std;


// VARDEF /////////////////////////////////////////////////////////////

VarDef::VarDef(shared_ptr<Id> i) : id(i) {}

VarDef::~VarDef() {}

void VarDef::visit(CodeGen* generator) {
  generator->visit(this);
}

void VarDef::display(ostream& out) const {
  out << *id;
}


// DEFSEQ /////////////////////////////////////////////////////////////

DefSeq::DefSeq(shared_ptr<Def> f, shared_ptr<Def> r)
    : first(f), rest(r) {}

DefSeq::~DefSeq() {}

void DefSeq::visit(CodeGen* generator) {
  generator->visit(this);
}

void DefSeq::display(ostream& out) const {
  out << *first << endl << *rest;
}


// PROCDEF ////////////////////////////////////////////////////////////

ProcDef::ProcDef(shared_ptr<Expr> d, shared_ptr<Stmt> s)
    : name(d), block(s)  {}

ProcDef::~ProcDef() {}

void ProcDef::visit(CodeGen* generator) {
  generator->visit(this);
}

void ProcDef::display(ostream& out) const {
  out << endl << "PROC" << endl << *name;
  out << *block << "ENDPROC";
}

