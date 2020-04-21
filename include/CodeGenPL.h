#ifndef PLCC_PL_ASM_H
#define PLCC_PL_ASM_H

#include "TreeWalker.h"
#include "AstNode.h"
#include "Defs.h"
#include "Exprs.h"
#include "Stmts.h"
#include <iostream>


class CodeGenPL : public TreeWalker {
 public:
  CodeGenPL(std::ostream* out);
  virtual ~CodeGenPL();
  void walk(AstNode& node);
  void visit(AstNode& node);

  // Def nodes
  void visit(DefSeq& node);
  void visit(VarDef& node);

  // Expr nodes
  void visit(Id& node);
  void visit(Constant& node);

  // Stmt nodes
  void visit(Block& node);
  void visit(Seq& node);
  void visit(IoStmt& node);

 private:
  std::ostream* out;
};

#endif
