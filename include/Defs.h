#ifndef PLCC_DEFS_H
#define PLCC_DEFS_H

#include "AstNode.h"
#include "Exprs.h"
#include "Stmts.h"
#include "TreeWalker.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>

class Def : public AstNode {
 public:
  Def();
  Def(std::shared_ptr<Id> id);
  virtual ~Def();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  virtual int get_size() { return 1; }

  virtual std::shared_ptr<Id> get_id() { return id; }

 protected:
  std::shared_ptr<Id> id;
};

// Now that all Defs have an id this can probably be gotten rid of
class VarDef : public Def {
 public:
  VarDef(std::shared_ptr<Id> i);
  virtual ~VarDef();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  virtual int get_size() { return id->get_size(); }
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


// To replace DefSeq eventually
class DefPart : public Def {
 public:
  DefPart();
  DefPart(std::shared_ptr<Def> def);
  virtual ~DefPart();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  virtual int get_size() { return defs.size(); }

  void add_defs(std::shared_ptr<DefPart> defs);
  void add_def(std::shared_ptr<Def> def) { defs.push_back(def); }
  std::vector<std::shared_ptr<Def>>& get_defs() { return defs; }

 protected:
  std::vector<std::shared_ptr<Def>> defs;
};


class ProcDef : public Def {
 public:
  ProcDef(std::shared_ptr<Id> name, std::shared_ptr<DefPart> params,
          std::shared_ptr<Block> block);
  virtual ~ProcDef();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;
  virtual int get_size() { return id->get_size(); }

  DefPart& get_params() { return *params; }
  Block& get_block() { return *block; }

 protected:
  std::shared_ptr<DefPart> params;
  std::shared_ptr<Block> block;
};

class RecDef : public Def {
 public:
  RecDef(std::shared_ptr<Id> name, std::shared_ptr<Def> defs);
  virtual ~RecDef();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& os) const;

  Def& get_defs() { return *defs; }

 protected:
  std::shared_ptr<Def> defs;
};
#endif
