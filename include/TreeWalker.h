#ifndef PLCC_TREE_WALKER_H
#define PLCC_TREE_WALKER_H

#include <memory>
#include <iostream>

class AstNode;
class DefSeq;
class VarDef;
class Constant;
class Id;
class Block;
class Seq;
class Write;
class IoStmt;


class TreeWalker {
 public:
  TreeWalker();
  virtual ~TreeWalker();
  virtual void walk(AstNode& tree);
  virtual void visit(AstNode& tree) {}

  // Def nodes
  virtual void visit(DefSeq& node) {}
  virtual void visit(VarDef& node) {}

  // Expr nodes
  virtual void visit(Id& node) {}
  virtual void visit(Constant& node) {}

  // Stmt nodes
  virtual void visit(Block& node) {}
  virtual void visit(Seq& node) {}
  virtual void visit(IoStmt& node) {}
};

#endif
