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
  Type full_type = Type();
};

struct TypeEntry {
  int size;
  std::map<std::string,std::pair<int,Type>> fields;
};


class CodeGenPL : public TreeWalker {
 public:
  CodeGenPL(std::shared_ptr<Admin> admin, std::ostream* out);
  virtual ~CodeGenPL();
  void walk(AstNode& node);
  void visit(AstNode& node);

  // Def nodes
  void visit(DefPart& node);
  void visit(VarDef& node);
  void visit(ProcDef& node);
  void visit(RecDef& node);

  // Expr nodes
  void visit(Id& node);
  void visit(ConstId& node);
  void visit(Constant& node);
  void visit(ConstString& node);
  void visit(Access& node);
  void visit(ArrayAccess& node);
  void visit(RecAccess& node);
  void visit(Binary& node);
  void visit(Unary& node);

  // Stmt nodes
  void visit(BlockStmt& node);
  void visit(Block& node);
  void visit(Seq& node);
  void visit(IoStmt& node);
  void visit(ReadLine& node);
  void visit(Asgn& node);
  void visit(StringAsgn& node);
  void visit(IfStmt& node);
  void visit(Loop& node);
  void visit(Cond& node);
  void visit(CondSeq& node);
  void visit(Proc& node);

 private:
  std::shared_ptr<Admin> admin;
  std::ostream* out;

  int current_address;
  std::vector<int> ops; 
  std::vector<int> jumps;
  std::vector<int> var_lengths;
  std::vector< std::map<std::string, TableEntry> > table;
  std::map<std::string, TypeEntry> types;
  std::vector<std::string> rec_types;

  enum Acs{DEF, VAR, VAL, REF, SIZE, CALL, REC, REC_DEF};
  Acs access;

  TableEntry table_find(std::string name);
};

#endif
