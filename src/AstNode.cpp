#include "AstNode.h"
#include "TreeWalker.h"
#include <iostream>
using namespace std;

// AST ////////////////////////////////////////////////////////////////
AstNode::AstNode() : name("Ast Node"), null(false) {}

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


// EXPR ///////////////////////////////////////////////////////////////
Expr::Expr(Type t) : type(t) {
  name = "Expr Node";
}

Expr::~Expr() {}

void Expr::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Expr::display(ostream& out) const {
  out << "(";
  if (type.type == symbol::RECORD)
    out << type.name;
  else
    out << symbol::to_string.at(type.type);
  out << ")";
}




// STMT ///////////////////////////////////////////////////////////////
Stmt::Stmt() {
  name = "EMPTY STMT";
}

Stmt::~Stmt() {}

void Stmt::visit(TreeWalker& walker) {
  walker.visit(*this);
}

void Stmt::display(ostream& out) const {
  AstNode::display(out);
}
