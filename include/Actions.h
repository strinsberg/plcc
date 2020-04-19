#ifndef PLCC_ACTIONS_H
#define PLCC_ACTIONS_H

#include "Admin.h"
#include "AstNode.h"
#include "Defs.h"
#include "Exprs.h"
#include "Stmts.h"
#include "BlockTable.h"
#include "Symbol.h"
#include "Types.h"
#include <vector>
#include <memory>

struct Vp {
  std::shared_ptr<Expr> size;
  std::vector<std::string> names;
};


class Actions {
 public:
  Actions(std::shared_ptr<Admin> admin);
  ~Actions();

  // Accessors
  std::shared_ptr<AstNode> get_ast() { return ast; }
  void set_ast(std::shared_ptr<AstNode> a) { ast = a; }
  std::shared_ptr<Admin> get_admin() { return admin; }


  // definition actions
  std::shared_ptr<Def> def_part(std::shared_ptr<Def>, std::shared_ptr<Def>);
  std::shared_ptr<Def> const_def(Type, std::string, std::shared_ptr<Expr>);
  std::shared_ptr<Def> var_def(Type, Vp);
  Vp vprime(std::vector<std::string>, std::shared_ptr<Expr> e = nullptr);
  std::shared_ptr<Def> array_def(int vars);
  std::shared_ptr<Def> proc_def(std::shared_ptr<Expr>, std::shared_ptr<Stmt>);
  std::shared_ptr<Expr> proc_name(std::string name);

  // statement actions
  std::shared_ptr<Stmt> block(std::shared_ptr<Def>, std::shared_ptr<Stmt>);
  std::shared_ptr<Stmt> stmt_part(std::shared_ptr<Stmt>, std::shared_ptr<Stmt>);
  std::shared_ptr<Stmt> io(std::vector<std::shared_ptr<Expr>>, symbol::Tag);
  std::shared_ptr<Stmt> assign(
    std::vector<std::shared_ptr<Expr>>, std::vector<std::shared_ptr<Expr>>
  );
  std::shared_ptr<Stmt> if_stmt(std::shared_ptr<Stmt>);
  std::shared_ptr<Stmt> loop(std::shared_ptr<Stmt>);
  std::shared_ptr<Stmt> empty_stmt();
  std::shared_ptr<Stmt> proc_stmt(std::string);
  std::shared_ptr<Stmt> conditions(std::shared_ptr<Stmt>, std::shared_ptr<Stmt>);
  std::shared_ptr<Stmt> condition(std::shared_ptr<Expr>, std::shared_ptr<Stmt>);

  // expression actions
  std::shared_ptr<Expr> access(std::string, std::shared_ptr<Expr>);
  std::shared_ptr<Expr> binary(
    Operator, std::shared_ptr<Expr>, std::shared_ptr<Expr>
  );
  std::shared_ptr<Expr> unary(symbol::Tag, std::shared_ptr<Expr>);
  std::shared_ptr<Expr> constant(symbol::Tag t, int val = 0, double dec = 0.0);
  std::shared_ptr<Expr> empty_expr() { return std::make_shared<Expr>( Expr(Type()) ); }

  // helpers
  Type new_type(symbol::Tag);
  Operator new_op(symbol::Tag, symbol::Tag, symbol::Tag=symbol::UNIVERSAL);
  void new_block();

 private:
  BlockTable table;
  std::shared_ptr<Admin> admin;
  std::shared_ptr<AstNode> ast;

  std::shared_ptr<Id> get_id(std::string);
  std::shared_ptr<Def> add_vars(
    std::vector<std::string> names, Type type, std::shared_ptr<Expr> size
  );
};

#endif
