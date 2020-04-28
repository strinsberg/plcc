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
  AstNode(std::string name);
  virtual ~AstNode();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;

  std::string get_name() { return name; }

  friend std::ostream& operator<< (std::ostream& out, const AstNode& node);

 protected:
  std::string name;
};


class DefPart : public AstNode {
 public:
  DefPart();
  DefPart(std::shared_ptr<Def> def);
  virtual ~DefPart();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;
  std::shared_ptr<Def> get_def(std::string name);
  void add_defs(std::shared_ptr<DefPart> defs);

  void add_def(std::shared_ptr<Def> def) { defs.push_back(def); }
  std::vector<std::shared_ptr<Def>>& get_defs() { return defs; }
  size_t get_size() { return defs.size(); }

 protected:
  std::vector<std::shared_ptr<Def>> defs;
};


class StmtPart : public AstNode {
 public:
  StmtPart();
  StmtPart(std::shared_ptr<Stmt> stmt);
  virtual ~StmtPart();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;
  void add_stmts(std::shared_ptr<Stmt> stmts);

  void add_stmt(std::shared_ptr<Stmt> stmt) { stmts.push_back(stmt); }
  std::vector<std::shared_ptr<Stmt>>& get_stmts() { return stmts; }
  size_t get_size() { return stmts.size(); }

 protected:
  std::vector<std::shared_ptr<Stmt>> stmts;
};


#endif
