#include "Defs.h"
#include "Exprs.h"
#include "TreeWalker.h"
#include <iostream>
#include <memory>
using namespace std;

// DEF ////////////////////////////////////////////////////////////////
Def::Def() {
  name = "EMPTY DEF";
  id = make_shared<Id>("null id", Type(), make_shared<Constant>());
}

Def::Def(shared_ptr<Id> i) : id(i) {
  name = id->get_name();
}

Def::~Def() {}

void Def::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Def::display(ostream& out) const {
  AstNode::display(out);
}

// VARDEF /////////////////////////////////////////////////////////////

VarDef::VarDef(shared_ptr<Id> i) : Def(i) {}

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


// DefPart ////////////////////////////////////////////////////////////
DefPart::DefPart() : Def() {
  name = "Def Part";
}

DefPart::DefPart(std::shared_ptr<Def> def) : Def() {
  name = "Def Part";
  defs.push_back(def);
}

DefPart::~DefPart() {}

void DefPart::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void DefPart::display(std::ostream& out) const {
  for (auto& def : defs)
    out << *def << endl;
}

void DefPart::add_defs(std::shared_ptr<DefPart> definitions) {
  for (auto def : definitions->get_defs())
    defs.push_back(def);
}


// PROCDEF ////////////////////////////////////////////////////////////

ProcDef::ProcDef(shared_ptr<Id> i, shared_ptr<Stmt> b)
    : Def(i), block(b)  {}

ProcDef::~ProcDef() {}

void ProcDef::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void ProcDef::display(ostream& out) const {
  out << endl << "PROC" << endl << *id;
  out << *block << "ENDPROC";
}

// Record Definition //////////////////////////////////////////////////

RecDef::RecDef(shared_ptr<Id> n, shared_ptr<Def> d) : Def(n), defs(d) {}

RecDef::~RecDef() {}

void RecDef::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void RecDef::display(std::ostream& out) const {
  out << "RECORD" << endl;
  out << *id << endl;
  out << *defs << endl << "ENDREC";
}

