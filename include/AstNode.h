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
  virtual void display(std::ostream& os) const;
  friend std::ostream& operator<< (std::ostream& out, const AstNode& node);
};


class Expr : public AstNode {
 public:
  Expr(Type type);
  virtual ~Expr();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;

  Type get_type() { return type; }

 protected:
  Type type;
};


class Def : public AstNode {
 public:
  Def();
  virtual ~Def();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;
};


class Stmt : public AstNode {
 public:
  Stmt();
  virtual ~Stmt();
  virtual void visit(CodeGen* generator);
  virtual void display(std::ostream& os) const;
};

#endif
