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
  Constant(int v = 1);
  Constant(Type type, int value = 0, int exp = 1);
  virtual ~Constant();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  int get_size() { return value; }

  int get_value() { return value; }
  int get_exp() { return exp; }

 protected:
  int value;
  int exp;
};


class ConstString : public Constant {
 public:
  ConstString(std::string);
  virtual ~ConstString();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  int get_size() { return text.size(); }

  std::string& get_string() { return text; }

 protected:
  std::string text;
};


class Id : public Expr {
 public:
  Id(std::string lexeme, Type type, std::shared_ptr<Expr> size);
  virtual ~Id();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  int get_size() { return size->get_size(); }

  Expr& get_size_expr() { return *size; }

 protected:
  std::shared_ptr<Expr> size;  // is it really safe for this to be Expr?
};


class ConstId : public Id {
 public:
  ConstId(std::string lexeme, Type type, std::shared_ptr<Expr> value);
  virtual ~ConstId();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

  Expr& get_value() { return *value; }

 protected:
  std::shared_ptr<Expr> value;
};


class Access : public Expr {
 public:
  Access(std::shared_ptr<Id> id);
  virtual ~Access();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  int get_size() { return id->get_size(); }

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


class RecAccess : public Expr {
 public:
  RecAccess(std::shared_ptr<Expr> record, std::shared_ptr<Expr> field);
  virtual ~RecAccess();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

  Expr& get_record() { return *record; }
  Expr& get_field() { return *field; }

 protected:
  std::shared_ptr<Expr> record;
  std::shared_ptr<Expr> field;
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

  Operator& get_op() { return op; }
  Expr& get_expr() { return *expr; }

 protected:
  Operator op;
  std::shared_ptr<Expr> expr;
};

#endif
