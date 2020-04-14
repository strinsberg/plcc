#ifndef PLCC_DEFS_H
#define PLCC_DEFS_H

#include "AstNode.h"
#include <string>


class Def : public AstNode {
 public:
  Def(int line) : AstNode(line) {} 
  virtual ~Def() {}  // Probably need to be responsible for deleteing ID's eventually
  virtual std::string to_string() { return "def: " + std::to_string(line_num); }
};

class VarDef : public Def {
 public:
  VarDef(Id* i, int line) : Def(line), id(i) {} 
  virtual ~VarDef() {}  // Probably need to be responsible for deleteing ID's eventually
  virtual std::string to_string() { return "Var: " + id->to_string(); }

  Id* id;
};

class DefSeq : public Def {
 public:
  DefSeq(Def* f, Def* r, int line) : Def(line), first(f), rest(r) {}
  virtual ~DefSeq() { delete rest; }
  virtual std::string to_string() { return first->to_string() + "\n" + rest->to_string(); }

  Def* first;
  Def* rest;
};

#endif
