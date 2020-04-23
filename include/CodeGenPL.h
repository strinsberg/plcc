#ifndef PLCC_PL_ASM_H
#define PLCC_PL_ASM_H

#include "TreeWalker.h"
#include "AstNode.h"
#include "Defs.h"
#include "Exprs.h"
#include "Stmts.h"
#include "Admin.h"
#include "Symbol.h"
#include <iostream>
#include <vector>
#include <map>
#include <sstream>


struct TableEntry {
  int address = 0;
  int block = 0;
  int displace = 0;
  symbol::Tag type = symbol::EMPTY;
};


class CodeGenPL : public TreeWalker {
 public:
  CodeGenPL(std::shared_ptr<Admin> admin, std::ostream* out);
  virtual ~CodeGenPL();
  void walk(AstNode& node);
  void visit(AstNode& node);

  // Def nodes
  void visit(DefSeq& node);
  void visit(VarDef& node);
  void visit(ProcDef& node);

  // Expr nodes
  void visit(Id& node);
  void visit(ConstId& node);
  void visit(Constant& node);
  void visit(Access& node);
  void visit(ArrayAccess& node);
  void visit(Binary& node);
  void visit(Unary& node);

  // Stmt nodes
  void visit(Block& node);
  void visit(Seq& node);
  void visit(IoStmt& node);
  void visit(Asgn& node);
  void visit(IfStmt& node);
  void visit(Loop& node);
  void visit(Cond& node);
  void visit(CondSeq& node);
  void visit(Proc& node);

 private:
  std::shared_ptr<Admin> admin;
  std::ostream* out;

  int current_address;
  std::vector<std::string> ops;  // Make int once all instructions are numbers
  std::vector<int> jumps;
  std::vector<int> var_lengths;
  std::vector< std::map<std::string, TableEntry> > table;

  enum Acs{DEF, VAR, VAL, SIZE, CALL};
  Acs access;

  TableEntry table_find(std::string name);
};

#endif
