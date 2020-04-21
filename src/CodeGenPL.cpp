#include "CodeGenPL.h"
#include "TreeWalker.h"
#include "AstNode.h"
#include "Defs.h"
#include "Exprs.h"
#include "Stmts.h"
#include <iostream>
using namespace std;


CodeGenPL::CodeGenPL(ostream* o) : TreeWalker(), out(o) {}

CodeGenPL::~CodeGenPL() {}


void CodeGenPL::walk(AstNode& node) {
  node.visit(*this);
}

void CodeGenPL::visit(AstNode& node) {
  *out << "GEN ASTNODE VISIT" << endl;
}


// Def nodes
void CodeGenPL::visit(DefSeq& node) {
  // Has a first and rest. Actions builds more like rest last.
  *out << "def seq" << endl;
}

void CodeGenPL::visit(VarDef& node) {
  // Has an Id member
  *out << "var def" << endl;
}


// Expr nodes
void CodeGenPL::visit(Id& node) {
  // Has a name, type, size(Constant)
  *out << "id" << endl;
}

void CodeGenPL::visit(Constant& node) {
  // Has a type, value, dec 
  *out << "constant" << endl;
}


// Stmt nodes
void CodeGenPL::visit(Block& node) {
  // Has Defs and Stmts. Both could be Seq, singular, or empty?
  *out << "block" << endl;
}

void CodeGenPL::visit(Seq& node) {
  // Has a first and rest like DefSeq
  *out << "seq" << endl;
}

void CodeGenPL::visit(IoStmt& node) {
  // Has an expr and a type tag
  *out << "io" << endl;
}
