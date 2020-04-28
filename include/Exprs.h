#ifndef PLCC_EXPRESSIONS_H
#define PLCC_EXPRESSIONS_H

#include "AstNode.h"
#include "TreeWalker.h"
#include "Symbol.h"
#include "Types.h"
#include <string>
#include <memory>


class Expr : public AstNode {
 public:
  Expr(Type type);
  virtual ~Expr();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

  Type get_type() { return type; }

 protected:
  Type type;
};


class Access : public Expr {
 public:
  Access(std::string name, int size, std::shared_ptr<Type> type);
  virtual ~Access();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

  virtual int get_size() { return size; }
  const std::string& get_name() { return name; }

 protected:
  std::string name;
  int size;
};


class ArrayAccess : public Access {
 public:
  ArrayAccess(std::string name, std::shared_ptr<Type> type, std::shared_ptr<Expr> index);
  virtual ~ArrayAccess();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

  std::shared_ptr<Expr> get_index() { return index; }

 protected:
  std::shared_ptr<Expr> index;
};


class RecAccess : public Access {
 public:
  RecAccess(std::string name, std::shared_ptr<Type> type);
  virtual ~RecAccess();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  void add_access(std::shared_ptr<Access> access);

  std::vector<std::shared_ptr<Access>>& get_acs() { return acs; }

 protected:
  std::vector<std::shared_ptr<Access>> acs;
};


class Binary : public Expr {
 public:
  Binary(Operator op, std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs);
  virtual ~Binary();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

  Operator& get_op() { return op; }
  std::shared_ptr<Expr> get_lhs() { return lhs; }
  std::shared_ptr<Expr> get_rhs() { return rhs; }

 protected:
  Operator op;
  std::shared_ptr<Expr> lhs;
  std::shared_ptr<Expr> rhs;
};


class Unary : public Expr {
 public:
  Unary(Operator op, std::shared_ptr<Expr> e);
  virtual ~Unary();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

  Operator& get_op() { return op; }
  std::shared_ptr<Expr> get_expr() { return expr; }

 protected:
  Operator op;
  std::shared_ptr<Expr> expr;
};

#endif
