#ifndef PLCC_TREE_WALKER_H
#define PLCC_TREE_WALKER_H

#include <memory>
#include <iostream>
class AstNode;

class TreeWalker {
 public:
  TreeWalker();
  virtual ~TreeWalker();
  virtual void walk(AstNode& tree);
  virtual void visit(AstNode& node);
};

#endif
