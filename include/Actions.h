#ifndef PLCC_ACTIONS_H
#define PLCC_ACTIONS_H

#include "AstNode.h"
#include "AstStacks.h"
#include "BlockTable.h"
#include "Symbol.h"
#include <vector>


class Actions {
 public:
  Actions();
  ~Actions();

  AstNode* ast() { return stacks.pop_stmt(); }

  // definition actions
  void new_token(tag::Tag tag, std::string lexeme="");
  void const_def(int line);
  void var_def(tag::Tag kind, int vars, int line);
  void array_def(int vars, int line);
  void def_part(int num_defs, int line);

  // stmt actions
  void block(int num_defs, int num_stmts, int line);
  void stmt_part(int num_stmts, int line);
  void write(int num_expr, int line);
  void assign(int vars, int exprs, int line);
  void if_stmt(int num_cond, int line);
  void loop(int line);
  void empty(int line);
  void condition(int num_stmts, int line);

  // expr actions
  void access(int line, tag::Tag type);
  void binary(int line);
  void unary(tag::Tag t, int line);
  void constant(tag::Tag t, int line, int val = 0, int dec = 0);

  // helpers
  void display();
  void new_block() { table.push_block(); }
  void error(std::string text, int line, std::string lexeme="");


 private:
  AstStacks stacks;
  BlockTable table;

  void add_vars(tag::Tag type, tag::Tag kind, int vars, int line);
};

#endif
