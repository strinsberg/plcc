#ifndef PLCC_ACTIONS_H
#define PLCC_ACTIONS_H

#include "Admin.h"
#include "AstNode.h"
#include "AstStacks.h"
#include "BlockTable.h"
#include "Symbol.h"
#include <vector>


class Actions {
 public:
  Actions(Admin* admin);
  ~Actions();

  AstNode* ast() { return stacks.pop_stmt(); }
  Admin* get_admin() {return admin; }

  // definition actions
  void new_token(tag::Tag tag, std::string lexeme="");
  void const_def();
  void var_def(tag::Tag kind, int vars);
  void array_def(int vars);
  void def_part(int num_defs);

  // stmt actions
  void block(int num_defs, int num_stmts);
  void stmt_part(int num_stmts);
  void write(int num_expr);
  void assign(int vars, int exprs);
  void if_stmt(int num_cond);
  void loop();
  void empty();
  void condition(int num_stmts);

  // expr actions
  void access(tag::Tag type);
  void binary();
  void unary(tag::Tag t);
  void constant(tag::Tag t, int val = 0, int dec = 0);

  // helpers
  void display();
  void new_block() { table.push_block(); }
  void newline() { line_num++; }
  int line() { return line_num; }


 private:
  AstStacks stacks;
  BlockTable table;
  Admin* admin;

  int line_num;

  void add_vars(tag::Tag type, tag::Tag kind, int vars);
};

#endif
