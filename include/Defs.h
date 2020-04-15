#ifndef PLCC_DEFS_H
#define PLCC_DEFS_H

#include "AstNode.h"
#include <string>


class VarDef : public Def {
 public:
  VarDef(Id* i);
  virtual ~VarDef();
  virtual void visit(CodeGen* generator);

 private:
  Id* id;
};

class DefSeq : public Def {
 public:
  DefSeq(Def* f, Def* r);
  virtual ~DefSeq();
  virtual void visit(CodeGen* generator);

 protected:
  Def* first;
  Def* rest;
};

#endif
