#include "Defs.h"
#include "Exprs.h"
#include "TreeWalker.h"
#include <iostream>
#include <memory>
using namespace std;


// VARDEF /////////////////////////////////////////////////////////////

VarDef::VarDef(shared_ptr<Id> i) : id(i) {}

VarDef::~VarDef() {}

void VarDef::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void VarDef::display(ostream& out) const {
  out << *id;
}


// DEFSEQ /////////////////////////////////////////////////////////////

DefSeq::DefSeq(shared_ptr<Def> f, shared_ptr<Def> r)
    : first(f), rest(r) {}

DefSeq::~DefSeq() {}

void DefSeq::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void DefSeq::display(ostream& out) const {
  out << *first << endl << *rest;
}


// PROCDEF ////////////////////////////////////////////////////////////

ProcDef::ProcDef(shared_ptr<Expr> d, shared_ptr<Stmt> s)
    : name(d), block(s)  {}

ProcDef::~ProcDef() {}

void ProcDef::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void ProcDef::display(ostream& out) const {
  out << endl << "PROC" << endl << *name;
  out << *block << "ENDPROC";
}

// Record Definition //////////////////////////////////////////////////

RecDef::RecDef(shared_ptr<Id> n, shared_ptr<Def> d) : Def(), name(n), defs(d) {}

RecDef::~RecDef() {}

void RecDef::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void RecDef::display(std::ostream& out) const {
  out << "RECORD" << endl;
  out << *name << endl;
  out << *defs << endl << "ENDREC";
}

