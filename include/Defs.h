#ifndef PLCC_DEFS_H
#define PLCC_DEFS_H

#include "AstNode.h"
#include "Type.h"
#include "TreeWalker.h"
#include <iostream>
#include <string>
#include <memory>
#include <set>


class Def : public AstNode {
 public:
  Def(std::shared_ptr<Type> type);
  virtual ~Def();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;
  virtual bool add_name(std::string name);
  virtual bool has_name(std::string name);

  virtual const std::set<std::string>& get_names() { return names; }
  virtual std::shared_ptr<Type> get_type() { return type; }
  virtual size_t get_size() { return names.size(); }

 protected:
  std::shared_ptr<Type> type;
  std::set<std::string> names;
};


class ConstDef : public Def {
 public:
  ConstDef(std::string name, std::shared_ptr<Type> type, std::shared_ptr<Literal> value);
  virtual ~ConstDef();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;
  virtual bool has_name(std::string name);

  std::shared_ptr<Literal> get_value() { return value; }

 protected:
  std::shared_ptr<Literal> value;
};


class ProcDef : public Def {
 public:
  ProcDef(std::string name, std::shared_ptr<Block> block);
  virtual ~ProcDef();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;
  virtual bool has_name(std::string name);

  virtual int get_size() { return block->get_defs()->get_size(); }
  virtual std::shared_ptr<Block> get_block() { return *block; }

 protected:
  std::shared_ptr<Block> block;
};


class RecDef : public Def {
 public:
  RecDef(std::string name, std::shared_ptr<DefPart> defs);
  virtual ~RecDef();
  virtual void visit(TreeWalker& walker);
  virtual void display(std::ostream& out) const;
  virtual bool has_name(std::string name);
  virtual bool has_field(std::string name);

  virtual std::shared_ptr<DefPart> get_defs() { return defs; }

 protected:
  std::shared_ptr<DefPart> defs;
};

#endif
