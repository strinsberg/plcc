#ifndef PLCC_STATEMENTS_H
#define PLCC_STATEMENTS_H

#include "AstNode.h"
#include <string>


class Stmt : public AstNode {
 public:
  Stmt(int line) : AstNode(line) {}
  virtual ~Stmt() {}
  virtual std::string to_string() { return "stmt: " + std::to_string(line_num); }
};
// if, loop, assign, write, skip
// condition?
// array assignment
// sequence of statments

#endif

