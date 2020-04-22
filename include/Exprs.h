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
  Constant(Type type, int value = 0, int dec = 0);
  virtual ~Constant();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

  int get_value() { return value; }
  int get_dec() { return dec; }

 private:
  int value;
  int dec;
};


class Id : public Expr {
 public:
  Id(std::string lexeme, Type type, std::shared_ptr<Expr> size);
  virtual ~Id();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

  Expr& get_size() { return *size; }

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

  Id& get_id() { return *id; }

 protected:
  std::shared_ptr<Id> id;
};


class ArrayAccess : public Access {
 public:
  ArrayAccess(std::shared_ptr<Id> id, std::shared_ptr<Expr> index);
  virtual ~ArrayAccess();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

  Expr& get_index() { return *index; }

 protected:
  std::shared_ptr<Expr> index;
};


class Binary : public Expr {
 public:
  Binary(Operator op, std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs);
  virtual ~Binary();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

  Operator& get_op() { return op; }
  Expr& get_lhs() { return *lhs; }
  Expr& get_rhs() { return *rhs; }

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
