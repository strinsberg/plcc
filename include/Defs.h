#ifndef PLCC_DEFS_H
#define PLCC_DEFS_H

#include "AstNode.h"
#include "Exprs.h"
#include "TreeWalker.h"
#include <iostream>
#include <string>
#include <memory>


class VarDef : public Def {
 public:
  VarDef(std::shared_ptr<Id> i);
  virtual ~VarDef();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  virtual int get_size() { return id->get_size(); }

  Id& get_id() { return *id; }

 protected:
  std::shared_ptr<Id> id;
};


class DefSeq : public Def {
 public:
  DefSeq(std::shared_ptr<Def> first, std::shared_ptr<Def> rest);
  virtual ~DefSeq();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  virtual int get_size() { return first->get_size() + rest->get_size(); }

  Def& get_first() { return *first; }
  Def& get_rest() { return *rest; }

 protected:
  std::shared_ptr<Def> first;
  std::shared_ptr<Def> rest;
};


class ProcDef : public Def {
 public:
  ProcDef(std::shared_ptr<Expr> name, std::shared_ptr<Stmt> block);
  virtual ~ProcDef();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  // Later should return the number of params. Probably stored in the name.
  virtual int get_size() { return name->get_size(); }

  Expr& get_id() { return *name; }
  Stmt& get_block() { return *block; }

 protected:
  std::shared_ptr<Expr> name;
  std::shared_ptr<Stmt> block;
};

class RecDef : public Def {
 public:
  RecDef(std::shared_ptr<Id> name, std::shared_ptr<Def> defs);
  virtual ~RecDef();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  virtual int get_size() { return name->get_size(); }

  Id& get_id() { return *name; }
  Def& get_defs() { return *defs; }

 protected:
  std::shared_ptr<Id> name;
  std::shared_ptr<Def> defs;
};
#endif
