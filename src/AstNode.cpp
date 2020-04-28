#include "AstNode.h"
#include "TreeWalker.h"
#include <iostream>
using namespace std;


// AST ////////////////////////////////////////////////////////////////

AstNode::AstNode() : name("AstNode") {}

AstNode::AstNode(std::string n) : name(n) {}

AstNode::~AstNode() {}

void AstNode::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void AstNode::display(ostream& out) const {
  out << name;
}

ostream& operator<< (ostream& out, const AstNode& node) {
  node.display(out);
  return out;
}


// DefPart ////////////////////////////////////////////////////////////

DefPart::DefPart() : AstNode("DefPart") {}

DefPart::DefPart(std::shared_ptr<Def> def) : DefPart() {
  defs.push_back(def);
}

DefPart::~DefPart() {}

void DefPart::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void DefPart::display(std::ostream& out) const {
  out << "DEF_PART" << endl;
  for (auto& def : defs)
    out << *def << endl;
}

std::shared_ptr<Def> DefPart::get_def(std::string name) {
  for (auto def : defs) {
    if (def.has_name(name))
      return def;
  }
  return nullptr;
}

void DefPart::add_defs(std::shared_ptr<DefPart> definitions) {
  for (auto def : definitions->get_defs())
    defs.push_back(def);
}


// STMT_PART //////////////////////////////////////////////////////////

StmtPart::StmtPart() : AstNode("StmtPart") {}

StmtPart::StmtPart(std::shared_ptr<Stmt> stmt) : StmtPart() {
  stmts.push_back(stmt);
}

StmtPart::~StmtPart() {}

void StmtPart::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void StmtPart::display(std::ostream& out) const {
  out << "STMT_PART" << endl;
  for (auto& stmt : stmts)
    out << *stmt << endl;
}

void StmtPart::add_stmts(std::shared_ptr<Stmt> statements) {
  for (auto stmt : statements->get_stmts())
    stmts.push_back(stmt);
}



