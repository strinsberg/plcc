#ifndef PLCC_AST_NODE_H
#define PLCC_AST_NODE_H

#include "Symbol.h"
#include "Tokens.h"
#include "CodeGen.h"
#include <string>
#include <iostream>


class AstNode {
 public:
  AstNode();
  virtual ~AstNode();

  virtual void visit(CodeGen* generator);
  friend std::ostream& operator<< (std::ostream& out, const AstNode& node);
};


class Expr : public AstNode {
 public:
  Expr();
  Expr(Token* token, symbol::Tag type);
  virtual ~Expr();
  virtual void visit(CodeGen* generator);

 protected:
  Token* token;
  symbol::Tag type;
};


class Def : public AstNode {
 public:
  Def();
  virtual ~Def();
  virtual void visit(CodeGen* generator);
};


class Stmt : public AstNode {
 public:
  Stmt();
  virtual ~Stmt();
  virtual void visit(CodeGen* generator);
};

#endif
