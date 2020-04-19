#include "Defs.h"
#include "Exprs.h"
#include <iostream>
#include <memory>
using namespace std;


// VARDEF /////////////////////////////////////////////////////////////
VarDef::VarDef(std::shared_ptr<Id> i) : id(i) {}

VarDef::~VarDef() {}

void VarDef::visit(CodeGen* generator) {
  generator->visit(this);
}

void VarDef::display(std::ostream& out) const {
  out << *id;
}


// DEFSEQ /////////////////////////////////////////////////////////////
DefSeq::DefSeq(std::shared_ptr<Def> f, std::shared_ptr<Def> r)
    : first(f), rest(r) {}

DefSeq::~DefSeq() {}

void DefSeq::visit(CodeGen* generator) {
  generator->visit(this);
}

void DefSeq::display(std::ostream& out) const {
  out << *first << endl << *rest;
}


// PROCDEF ////////////////////////////////////////////////////////////
ProcDef::ProcDef(std::shared_ptr<Expr> d, std::shared_ptr<Stmt> s)
    : name(d), block(s)  {}

ProcDef::~ProcDef() {}

void ProcDef::visit(CodeGen* generator) {
  generator->visit(this);
}

void ProcDef::display(std::ostream& out) const {
  out << endl << "PROC" << endl << *name;
  out << *block << "ENDPROC";
}

