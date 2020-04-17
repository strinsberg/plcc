#ifndef PLCC_AST_STACKS_H
#define PLCC_AST_STACKS_H

#include "Symbol.h"
#include "Tokens.h"
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
  void push_token(symbol::Tag t);
  void push_word(std::string lexeme);
  void push_def(Def* d) { defs.push_back(d); }
  void push_expr(Expr* d) { exprs.push_back(d); }
  void push_stmt(Stmt* d) { stmts.push_back(d); }

  // Type and Op
  void set_type(Type t);
  void set_op(Operator o);
  Type get_type();
  Operator get_op();

  // Get nodes
  Token* get_token(int from_top);
  Token* pop_token();
  Expr* pop_expr();
  Stmt* pop_stmt();
  Def* pop_def();

  // Display
  void print_tokens();
  void print_nodes();

 private:
  std::vector<Token*> tokens;
  std::vector<Expr*> exprs;
  std::vector<Stmt*> stmts;
  std::vector<Def*> defs;
  Type type;
  Operator op;
};

#endif
