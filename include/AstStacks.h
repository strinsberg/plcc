#ifndef PLCC_AST_STACKS_H
#define PLCC_AST_STACKS_H

#include "Symbol.h"
#include "Types.h"
#include "Exprs.h"
#include "Stmts.h"
#include "Defs.h"
#include <string>
#include <vector>


class AstStacks {
 public:
  AstStacks();
  ~AstStacks();

  // Add Nodes
  void push_op(Operator o) { ops.push_back(o); }
  void push_def(Def* d) { defs.push_back(d); }
  void push_expr(Expr* d) { exprs.push_back(d); }
  void push_stmt(Stmt* d) { stmts.push_back(d); }

  // Type
  void set_type(Type t);
  Type get_type();

  // Get nodes
  Operator pop_op();
  Expr* pop_expr();
  Stmt* pop_stmt();
  Def* pop_def();

  // Display
  void print_nodes();

 private:
  std::vector<Operator> ops;
  std::vector<Expr*> exprs;
  std::vector<Stmt*> stmts;
  std::vector<Def*> defs;
  Type type;
  Operator op;
};

#endif
