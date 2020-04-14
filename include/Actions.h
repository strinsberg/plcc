#ifndef PLCC_ACTIONS_H
#define PLCC_ACTIONS_H

#include "Tokens.h"
#include "Exprs.h"
#include "Stmts.h"
#include "Defs.h"
#include "BlockTable.h"
#include "parser.tab.h"
#include <vector>


class Actions {
 public:
  Actions() {}
  ~Actions();

  // Add tokens
  void add_t(yytokentype t);
  void add_w(std::string s);
  void add_n(int n);
  void add_f(int n, int d);
  void add_c(char c);

  // definition actions
  void const_def(int line);
  void var_def(yytokentype kind, int vars, int line);
  void array_def(int vars, int line);
  void def_part(int num_defs, int line);

  // stmt actions
  void block(int line);
  void stmt_part(int num_stmts, int line);
  void assign(int vars, int exprs, int line);

  // expr actions
  void access(int line, yytokentype type);
  void binary(int line);
  void unary(yytokentype t, int line);
  void constant(yytokentype t, int line);

  // helpers
  void print_tokens();
  void print_nodes();
  void print_table();

 private:
  std::vector<Token*> tokens;
  std::vector<Expr*> exprs;
  std::vector<Stmt*> stmts;
  std::vector<Def*> defs;
  BlockTable table;

  void add_vars(yytokentype type, yytokentype kind, int vars, int line);
  Token* next_token();
  Expr* next_expr();
  Stmt* next_stmt();
  Def* next_def();
};

#endif
