#include "Defs.h"
#include "Exprs.h"
#include "TreeWalker.h"
#include <iostream>
#include <memory>
using namespace std;


// DEF ////////////////////////////////////////////////////////////////

Def::Def(shared_ptr<Type> t) : AstNode("Def"), type(t) {}

Def::~Def() {}

void Def::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Def::display(ostream& out) const {
  out << *type << ":";
  for (auto& n : names)
    out << n << " ";
}

bool add_name(std::string name) {
  if (names.count(name))
    return false;

  names.insert(name);
  return true;
}

bool Def::has_name(std::string name) {
  return names.count(name);
}

// Const Def //////////////////////////////////////////////////////////

ConstDef::ConstDef(std::string n, shared_ptr<Type> t, shared_ptr<Literal> v)
    : Def(t), value(v) {
  name = n;
}

~ConstDef::ConstDef() {}

void ConstDef::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void ConstDef::display(std::ostream& out) const {
  Def::display(out);
  out << "(" << *value << ")";
}

bool ConstDef::has_name(std::string n) {
  return n == name;
}

// PROCDEF ////////////////////////////////////////////////////////////

ProcDef::ProcDef(string n, shared_ptr<Block> b)
    : Def( make_shared<Type>() ), block(b)  {
  // Type needs to be set to a type appropriate for a procedure
  // Once type has been re-worked to hold the right things
  name = n;
}

ProcDef::~ProcDef() {}

void ProcDef::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void ProcDef::display(ostream& out) const {
  out << endl << "PROC" << endl;
  out << *id << endl;
  out << *block << "ENDPROC";
}

bool ProcDef::has_name(std::string n) {
  return n == name;
}


// RECDEF /////////////////////////////////////////////////////////////

RecDef::RecDef(std::string n, shared_ptr<DefPart> d)
    : Def( make_shared<Type>() ), defs(d) {
  // Type needs to be created appropriately for a record
  name = n;
}

RecDef::~RecDef() {}

void RecDef::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void RecDef::display(std::ostream& out) const {
  out << "RECORD" << endl;
  out << *id << endl;
  out << *defs << endl << "ENDREC";
}

bool RecDef::has_name(std::string name) {
  return defs->get_def(name) == nullptr;
}


