#include "CodeGenPL.h"
#include "TreeWalker.h"
#include "AstNode.h"
#include "Defs.h"
#include "Exprs.h"
#include "Stmts.h"
#include "Admin.h"
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <map>
using namespace std;


CodeGenPL::CodeGenPL(shared_ptr<Admin> a, ostream* o)
    : TreeWalker(), admin(a), out(o), current_address(0), access(DEF) {}

CodeGenPL::~CodeGenPL() {}


void CodeGenPL::walk(AstNode& node) {
  *out << "PROG" << endl;
  current_address += 3;
  node.visit(*this);
  *out << "ENDPROG" << endl;  // The OP_NAMES need to be converted to numbers
}

void CodeGenPL::visit(AstNode& node) {
  admin->debug("ASTNODE VISIT");
}


// Def nodes
void CodeGenPL::visit(DefSeq& node) {
  // Has a first and rest. Actions builds more like rest last.
  admin->debug("def seq");
  node.get_first().visit(*this);
  node.get_rest().visit(*this);
}

void CodeGenPL::visit(VarDef& node) {
  // Has an Id member
  admin->debug("var def");
  access = DEF;
  node.get_id().visit(*this);
  var_lengths.back()++;
}


// Expr nodes
void CodeGenPL::visit(Id& node) {
  // Has a name, type, size(Constant)
  string name = node.get_name();
  admin->debug("id = " + name);

  if (access == DEF) {
    TableEntry ent;
    ent.address = current_address;
    ent.block = table.size() - 1;
    ent.displace = var_lengths.back() + 3;
    ent.type = node.get_type().type;

    table.back()[name] = ent; 
  } else {
    TableEntry ent = table.back()[name];

    *out << "VARIABLE" << endl;
    *out << table.size() - 1 - ent.block << endl;
    *out << ent.displace << endl;

    if (access == VAL) {
      *out << "VALUE" << endl; 
    }
  }
}

void CodeGenPL::visit(Constant& node) {
  // Has a type, value, dec 
  admin->debug("constant");
  // will need to access type when we do more than ints

  int value = node.get_value();
  *out << "CONSTANT" << endl;
  *out << value << endl;
}

void CodeGenPL::visit(Access& node) {
  // Has a type, value, dec 
  admin->debug("access");
  node.get_id().visit(*this);
}

// Stmt nodes
void CodeGenPL::visit(Block& node) {
  // Has Defs and Stmts. Both could be Seq, singular, or empty?
  admin->debug("block");

  var_lengths.push_back(0);
  table.push_back(map<string, TableEntry>());

  node.get_defs().visit(*this);
  *out << var_lengths.back() << endl;
  *out << current_address << endl;

  node.get_stmts().visit(*this);

  admin->debug(to_string(var_lengths.back()));
  table.pop_back();
  var_lengths.pop_back();
}

void CodeGenPL::visit(Seq& node) {
  // Has a first and rest like DefSeq
  admin->debug("seq");
  node.get_first().visit(*this);
  node.get_rest().visit(*this);
}

void CodeGenPL::visit(IoStmt& node) {
  // Has an expr and a type tag
  admin->debug("io");
  access = VAL;
  node.get_expr().visit(*this);

  *out << symbol::str(node.get_io_type()) << endl;
  *out << 1 << endl;
}

void CodeGenPL::visit(Asgn& node) {
  // Has an access and expr
  admin->debug("assign");

  access = VAR;
  node.get_acs().visit(*this);

  access = VAL;
  node.get_expr().visit(*this);

  *out << "ASSIGN" << endl;
  *out << 1 << endl;
}
