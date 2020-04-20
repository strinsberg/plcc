#ifndef PLCC_EXPRESSIONS_H
#define PLCC_EXPRESSIONS_H

#include "AstNode.h"
#include "TreeWalker.h"
#include "Symbol.h"
#include "Types.h"
#include <string>
#include <memory>


class Constant : public Expr {
 public:
  Constant();
  Constant(Type type, int value = 0, double dec = 0.0);
  virtual ~Constant();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

 private:
  int value;
  double dec;
};


class Id : public Expr {
 public:
  Id(std::string lexeme, Type type, std::shared_ptr<Expr> size);
  virtual ~Id();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
 protected:
  std::shared_ptr<Expr> size;
};


class ConstId : public Id {
 public:
  ConstId(std::string lexeme, Type type, std::shared_ptr<Expr> value);
  virtual ~ConstId();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
 protected:
  std::shared_ptr<Expr> value;
};


class Access : public Expr {
 public:
  Access(std::shared_ptr<Id> id);
  virtual ~Access();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

 protected:
  std::shared_ptr<Id> id;
};


class ArrayAccess : public Access {
 public:
  ArrayAccess(std::shared_ptr<Id> id, std::shared_ptr<Expr> index);
  virtual ~ArrayAccess();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

 protected:
  std::shared_ptr<Expr> index;
};


class Binary : public Expr {
 public:
  Binary(Operator op, std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs);
  virtual ~Binary();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

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

 protected:
  Operator op;
  std::shared_ptr<Expr> expr;
};

#endif
