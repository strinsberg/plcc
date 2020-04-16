#include "AstNode.h"
#include <iostream>
using namespace std;

// AST ////////////////////////////////////////////////////////////////
AstNode::AstNode() {}

AstNode::~AstNode() {}

void AstNode::visit(CodeGen* generator) {
  generator->visit(this);
}

void AstNode::display(ostream& out) const {
  out << "Ast Node";
}

std::ostream& operator<< (std::ostream& out, const AstNode& node) {
  node.display(out);
  return out;
}


// EXPR ///////////////////////////////////////////////////////////////
Expr::Expr() : token( new Token(symbol::EMPTY) ), type(symbol::EMPTY) {}

Expr::Expr(Token* tok, symbol::Tag tp) : token(tok), type(tp) {}

Expr::~Expr() {
  delete token;
}

void Expr::visit(CodeGen* generator) {
  generator->visit(this);
}

void Expr::display(ostream& out) const {
  out << token->to_string() << "(" << symbol::to_string.at(type);
  out << ")";
}


// DEF ////////////////////////////////////////////////////////////////
Def::Def() {}

Def::~Def() {}

void Def::visit(CodeGen* generator) {
  generator->visit(this);
}

void Def::display(ostream& out) const {
  out << "Def Node";
}


// STMT ///////////////////////////////////////////////////////////////
Stmt::Stmt() {}

Stmt::~Stmt() {}

void Stmt::visit(CodeGen* generator) {
  generator->visit(this);
}

void Stmt::display(ostream& out) const {
  out << "Stmt Node";
}
