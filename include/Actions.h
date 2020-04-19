#ifndef PLCC_ACTIONS_H
#define PLCC_ACTIONS_H

#include "Admin.h"
#include "AstNode.h"
#include "AstStacks.h"
#include "BlockTable.h"
#include "Symbol.h"
#include "Types.h"
#include <vector>


class Actions {
 public:
  Actions(Admin* admin);
  ~Actions();

  AstNode* get_ast() { return ast; }
  void set_ast(AstNode* a) { ast = a; }
  Admin* get_admin() { return admin; }

  Type* new_type(symbol::Tag);
  Operator* new_op(symbol::Tag, symbol::Tag, symbol::Tag=symbol::UNIVERSAL);

  // definition actions
  Def* def_part(Def*, Def*);
  Def* const_def(Type*, std::string*, Expr*);
  Def* var_def(Type*, std::pair<Expr*, std::vector<std::string*>*>*);
  std::pair<Expr*, std::vector<std::string*>*>* vprime(std::vector<std::string*>*, Expr* e = nullptr);
  Def* array_def(int vars);
  Def* proc_def(Expr*, Stmt*);
  Expr* proc_name(std::string* name);

  // stmt actions
  Stmt* block(Def*, Stmt*);
  Stmt* stmt_part(Stmt*, Stmt*);
  Stmt* io(std::vector<Expr*>*, symbol::Tag);
  Stmt* assign(std::vector<Expr*>*, std::vector<Expr*>*);
  Stmt* if_stmt(Stmt*);
  Stmt* loop(Stmt*);
  Stmt* empty_stmt();
  Stmt* proc_stmt(std::string*);
  Stmt* conditions(Stmt*, Stmt*);
  Stmt* condition(Expr*, Stmt*);

  // expr actions
  std::vector<Expr*>* expr_list(std::vector<Expr*>*, Expr*);
  Expr* access(std::string*, Expr*);
  Expr* binary(Operator*, Expr*, Expr*);
  Expr* unary(symbol::Tag, Expr*);
  Expr* constant(symbol::Tag t, int val = 0, double dec = 0.0);
  Expr* empty_expr() { return new Expr(Type()); }

  std::vector<std::string*>* var_list(std::vector<std::string*>* , std::string*);

  // helpers
  void display();
  Def* new_block() { admin->debug("new_block"); table.push_block(); return new Def(); }


 private:
  AstStacks stacks;
  BlockTable table;
  Admin* admin;
  AstNode* ast;

  Id* get_id(std::string);
  Def* add_vars(std::vector<std::string*>* names, Type* type, Expr* size);
};

#endif
