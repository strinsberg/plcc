#ifndef PLCC_AST_NODE_H
#define PLCC_AST_NODE_H

#include "Symbol.h"
#include "CodeGen.h"
#include <string>
#include <iostream>


class AstNode {
 public:
  AstNode();
  virtual ~AstNode();

  virtual visit(CodeGen* generator);
  friend std::ostream& operator<< (std::ostream& out, const AstNode& node);
};


class Expr : public AstNode {
 public:
  Expr();
  virtual ~Expr();
  symbol::Tag type;
};


class Def : public AstNode {
 public:
  Def();
  virtual ~Def();
};


class Stmt : public AstNode {
 public:
  Stmt();
  virtual ~Stmt();
};

#endif
