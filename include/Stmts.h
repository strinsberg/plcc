#ifndef PLCC_STATEMENTS_H
#define PLCC_STATEMENTS_H

#include "AstNode.h"
#include "Exprs.h"
#include "Defs.h"
#include "TreeWalker.h"
#include "Symbol.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>


class Seq : public Stmt {
 public:
  Seq(std::shared_ptr<Stmt> first, std::shared_ptr<Stmt> rest);
  virtual ~Seq();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;

  Stmt& get_first() { return *first; }
  Stmt& get_rest() { return *rest; }

 protected:
  std::shared_ptr<Stmt> first;
  std::shared_ptr<Stmt> rest;
};


class BlockStmt : public Stmt {
 public:
  BlockStmt(std::shared_ptr<Block> block);
  virtual ~BlockStmt();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;

  Block& get_block() { return *block; }

 protected:
  std::shared_ptr<Block> block;

};

class Block : public Stmt {
 public:
  Block(std::shared_ptr<DefPart> defs, std::shared_ptr<Stmt> stmts);
  virtual ~Block();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;

  DefPart& get_defs() { return *defs; }
  Stmt& get_stmts() { return *stmts; }

 protected:
  std::shared_ptr<DefPart> defs;
  std::shared_ptr<Stmt> stmts;
};


class Asgn : public Stmt {
 public:
  Asgn(std::shared_ptr<Expr> access, std::shared_ptr<Expr> expr);
  virtual ~Asgn();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;

  Expr& get_acs() { return *acs; }
  Expr& get_expr() { return *expr; }

 protected:
  std::shared_ptr<Expr> acs;
  std::shared_ptr<Expr> expr;
};

class StringAsgn : public Stmt {
 public:
  StringAsgn(std::shared_ptr<Expr> access, std::shared_ptr<Expr> expr);
  virtual ~StringAsgn();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;

  Expr& get_acs() { return *acs; }
  Expr& get_str() { return *str; }

 protected:
  std::shared_ptr<Expr> acs;
  std::shared_ptr<Expr> str;
};

class IoStmt : public Stmt {
 public:
  IoStmt(std::shared_ptr<Expr> expr, symbol::Tag type);
  virtual ~IoStmt();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;

  Expr& get_expr() { return *expr; }
  symbol::Tag get_io_type() { return type; }

 protected:
  std::shared_ptr<Expr> expr;
  symbol::Tag type;
};


class ReadLine : public Stmt {
 public:
  ReadLine(std::shared_ptr<Expr> array_id);
  virtual ~ReadLine();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;

  Expr& get_array_id() { return *array_id; }

 protected:
  std::shared_ptr<Expr> array_id;
};


class CondSeq : public Seq {
 public:
  CondSeq(std::shared_ptr<Stmt> first, std::shared_ptr<Stmt> rest);
  virtual ~CondSeq();
  virtual void visit(TreeWalker& walker);
};


class Cond : public Stmt {
 public:
  Cond(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> stmts);
  virtual ~Cond();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;

  Expr& get_cond() { return *cond; }
  Stmt& get_stmts() { return *stmts; }

 protected:
  std::shared_ptr<Expr> cond;
  std::shared_ptr<Stmt> stmts;
};


class Loop : public Stmt {
 public:
  Loop(std::shared_ptr<Stmt> condition);
  virtual ~Loop();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;

  Stmt& get_cond() { return *cond; }

 protected:
  std::shared_ptr<Stmt> cond;
};


class IfStmt : public Stmt {
 public:
  IfStmt(std::shared_ptr<Stmt> conditions);
  virtual ~IfStmt();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;

  Stmt& get_conds() { return *conds; }

 protected:
  std::shared_ptr<Stmt> conds;
};


class Proc : public Stmt {
 public:
  Proc(std::shared_ptr<Id> id, std::vector<std::shared_ptr<Expr>> args);
  virtual ~Proc();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;

  Expr& get_id() { return *id; }
  std::vector<std::shared_ptr<Expr>>& get_args() { return args; };

 protected:
  std::shared_ptr<Id> id;
  std::vector<std::shared_ptr<Expr>> args;
};
#endif

