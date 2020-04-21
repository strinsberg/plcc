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
    : TreeWalker(), admin(a), out(o), current_address(0), is_access(false) {}

CodeGenPL::~CodeGenPL() {}


void CodeGenPL::walk(AstNode& node) {
  node.visit(*this);
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
  var_lengths.back()++;
}


// Expr nodes
void CodeGenPL::visit(Id& node) {
  // Has a name, type, size(Constant)
  admin->debug("id");

  if (!is_access) {
    TableEntry ent;
    ent.address = current_address;
    ent.block = table.size() - 1;
    ent.displace = var_lengths.back() + 2;
    ent.type = node.get_type().type;

    table.back()[node.get_name()] = ent; 
  }
}

void CodeGenPL::visit(Constant& node) {
  // Has a type, value, dec 
  admin->debug("constant");
}


// Stmt nodes
void CodeGenPL::visit(Block& node) {
  // Has Defs and Stmts. Both could be Seq, singular, or empty?
  admin->debug("block");
  if (current_address == 0)
    *out << "PROG" << endl;
  current_address += 3;

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
}

void CodeGenPL::visit(IoStmt& node) {
  // Has an expr and a type tag
  admin->debug("io");
}
