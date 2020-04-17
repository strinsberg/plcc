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

  void new_type(symbol::Tag);
  void new_op(symbol::Tag, symbol::Tag);

  // definition actions
  void new_token(symbol::Tag tag, std::string lexeme="");
  void const_def();
  void var_def(symbol::Tag kind, int vars);
  void array_def(int vars);
  void proc_def();
  void def_part(int num_defs);
  void add_vars(Type type, symbol::Tag kind, int vars);

  // stmt actions
  void block(int num_defs, int num_stmts);
  void stmt_part(int num_stmts);
  void io(int num_expr, symbol::Tag);
  void assign(int vars, int exprs);
  void if_stmt(int num_cond);
  void loop();
  void empty();
  void proc_stmt();
  void condition(int num_stmts);

  // expr actions
  void access(symbol::Tag type);
  void binary();
  void unary();
  void constant(symbol::Tag t, int val = 0, double dec = 0.0);

  // helpers
  void display();
  void new_block() { admin->debug("new_block"); table.push_block(); }


 private:
  AstStacks stacks;
  BlockTable table;
  Admin* admin;
};

#endif
