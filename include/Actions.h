#ifndef PLCC_ACTIONS_H
#define PLCC_ACTIONS_H

#include "Tokens.h"
#include "Exprs.h"
#include "Stmts.h"
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

  // add ast nodes
  void const_def(int line);
  void var_def(int vars, int line);
  void array_def(int vars, int line);

  // helpers
  void print_tokens();
  void print_nodes();

 private:
  std::vector<Token*> tokens;
  std::vector<Expr*> exprs;
  std::vector<Stmt*> stmts;

};

#endif
