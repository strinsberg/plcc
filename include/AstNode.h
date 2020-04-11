#ifndef PLCC_AST_NODE_H
#define PLCC_AST_NODE_H

#include <string>


class AstNode {
 public:
  AstNode(int line);
  virtual ~AstNode();

  virtual int new_label();
  virtual void emit_label(int i);
  virtual void emit(std::string s);
  void error(std::string s);

  static int label;
  int line_num;
};

#endif
