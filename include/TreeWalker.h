#ifndef PLCC_TREE_WALKER_H
#define PLCC_TREE_WALKER_H

#include <memory>
#include <iostream>

// To avoid circular includes
class AstNode;
class DefSeq;
class VarDef;
class Constant;
class Id;
class Access;
class Block;
class Seq;
class Write;
class IoStmt;
class Asgn;


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
  virtual void visit(Access& node) {}

  // Stmt nodes
  virtual void visit(Block& node) {}
  virtual void visit(Seq& node) {}
  virtual void visit(IoStmt& node) {}
  virtual void visit(Asgn& node) {}
};

#endif
