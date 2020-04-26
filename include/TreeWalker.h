#ifndef PLCC_TREE_WALKER_H
#define PLCC_TREE_WALKER_H

#include <memory>
#include <iostream>

// Declar Node Classes to avoid circular includes
class AstNode;
class DefSeq;
class VarDef;
class Constant;
class ConstString;
class Id;
class ConstId;
class Access;
class ArrayAccess;
class BlockStmt;
class Block;
class Seq;
class Write;
class IoStmt;
class ReadLine;
class Asgn;
class StringAsgn;
class IfStmt;
class CondSeq;
class Cond;
class Binary;
class Unary;
class Loop;
class ProcDef;
class Proc;


class TreeWalker {
 public:
  TreeWalker();
  virtual ~TreeWalker();
  virtual void walk(AstNode& tree);
  virtual void visit(AstNode& tree) {}

  // Def nodes
  virtual void visit(DefSeq& node) {}
  virtual void visit(VarDef& node) {}
  virtual void visit(ProcDef& node) {}

  // Expr nodes
  virtual void visit(Id& node) {}
  virtual void visit(ConstId& node) {}
  virtual void visit(Constant& node) {}
  virtual void visit(ConstString& node) {}
  virtual void visit(Access& node) {}
  virtual void visit(ArrayAccess& node) {}
  virtual void visit(Binary& node) {}
  virtual void visit(Unary& node) {}

  // Stmt nodes
  virtual void visit(BlockStmt& node) {}
  virtual void visit(Block& node) {}
  virtual void visit(Seq& node) {}
  virtual void visit(IoStmt& node) {}
  virtual void visit(ReadLine& node) {}
  virtual void visit(Asgn& node) {}
  virtual void visit(StringAsgn& node) {}
  virtual void visit(IfStmt& node) {}
  virtual void visit(Loop& node) {}
  virtual void visit(Cond& node) {}
  virtual void visit(CondSeq& node) {}
  virtual void visit(Proc& node) {}

};

#endif
