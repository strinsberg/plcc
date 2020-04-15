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

std::ostream& operator<< (std::ostream& out, const VarDef& node) {
  out << "Var: " << node.id;
  return out;
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

std::ostream& operator<< (std::ostream& out, const DefSeq& node) {
  out << node.first << endl << node.rest;
  return out;
}
