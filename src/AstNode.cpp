#include "AstNode.h"
#include <iostream>
using namespace std;

// AST ////////////////////////////////////////////////////////////////
AstNode::AstNode() : name("Ast Node") {}

AstNode::~AstNode() {}

void AstNode::visit(CodeGen* generator) {
  generator->visit(this);
}

void AstNode::display(ostream& out) const {
  out << name;
}

std::ostream& operator<< (std::ostream& out, const AstNode& node) {
  node.display(out);
  return out;
}


// EXPR ///////////////////////////////////////////////////////////////
Expr::Expr(Type t) : type(t) {
  name = "Expr Node";
}

Expr::~Expr() {}

void Expr::visit(CodeGen* generator) {
  generator->visit(this);
}

void Expr::display(ostream& out) const {
  out << "(" << symbol::to_string.at(type.type) << ")";
}


// DEF ////////////////////////////////////////////////////////////////
Def::Def() {
  name = "EMPTY DEF";
}

Def::~Def() {}

void Def::visit(CodeGen* generator) {
  generator->visit(this);
}

void Def::display(ostream& out) const {
  AstNode::display(out);
}


// STMT ///////////////////////////////////////////////////////////////
Stmt::Stmt() {
  name = "EMPTY STMT";
}

Stmt::~Stmt() {}

void Stmt::visit(CodeGen* generator) {
  generator->visit(this);
}

void Stmt::display(ostream& out) const {
  AstNode::display(out);
}
