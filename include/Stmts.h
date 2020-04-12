#ifndef PLCC_STATEMENTS_H
#define PLCC_STATEMENTS_H

#include "AstNode.h"

// classes needed:
// stmt
class Stmt : public AstNode {
 public:
  Stmt(int line) : AstNode(line) {}
  virtual ~Stmt() {}
};
// if, loop, assign, write, skip
// condition?
// array assignment
// sequence of statments

#endif

