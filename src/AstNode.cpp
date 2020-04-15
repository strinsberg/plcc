#include "AstNode.h"
#include <iostream>
using namespace std;

// AST ////////////////////////////////////////////////////////////////
AstNode::AstNode() {}

AstNode::~AstNode() {}

void AstNode::visit(CodeGen* generator) {
  generator->visit(this);
}

std::ostream& operator<< (std::ostream& out, const AstNode& node) {
  out << "Ast Node" << endl;
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

std::ostream& operator<< (std::ostream& out, const Expr& node) {
  out << node.token->to_string() << " (" << symbol::to_string.at(node.type);
  out << ")" << endl;
  return out;
}


// DEF ////////////////////////////////////////////////////////////////
Def::Def() {}

Def::~Def() {}

void Def::visit(CodeGen* generator) {
  generator->visit(this);
}

std::ostream& operator<< (std::ostream& out, const Def& node) {
  out << "Def Node" << endl;
  return out;
}

// STMT ///////////////////////////////////////////////////////////////
Stmt::Stmt() {}

Stmt::~Stmt() {}

void Stmt::visit(CodeGen* generator) {
  generator->visit(this);
}

std::ostream& operator<< (std::ostream& out, const Stmt& node) {
  out << "Stmt Node" << endl;
  return out;
}
