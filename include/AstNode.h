#ifndef PLCC_AST_NODE_H
#define PLCC_AST_NODE_H

#include "Symbol.h"
#include "Types.h"
#include "TreeWalker.h"
#include <string>
#include <iostream>
#include <memory>


class AstNode {
 public:
  AstNode();
  virtual ~AstNode();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  bool is_null() { return null; }
  void set_null(bool n) { null = n; }
  std::string get_name() { return name; }
  void set_name(std::string n) { name = n; }
  friend std::ostream& operator<< (std::ostream& out, const AstNode& node);

 protected:
  std::string name;
  bool null;
};


class Expr : public AstNode {
 public:
  Expr(Type type);
  virtual ~Expr();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  virtual int get_size() { return 1; }

  Type get_type() { return type; }
  void set_type(Type t) { type = t; }

 protected:
  Type type;
};


class Def : public AstNode {
 public:
  Def();
  virtual ~Def();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
};


class Stmt : public AstNode {
 public:
  Stmt();
  virtual ~Stmt();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
};

#endif
