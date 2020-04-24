#include "CodeGenPL.h"
#include "TreeWalker.h"
#include "AstNode.h"
#include "Defs.h"
#include "Exprs.h"
#include "Stmts.h"
#include "Admin.h"
#include "exceptions.h"
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
  ops.push_back("PROG");
  current_address += 1;
  node.visit(*this);
  ops.push_back("ENDPROG");  // The OP_NAMES need to be converted to numbers

  for (auto & op : ops)
    *out << op << endl;
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

  if (node.get_id().get_type().qual != symbol::CONST) {
    access = SIZE;
    node.get_id().get_size().visit(*this);  // access the size member
  }
}


void CodeGenPL::visit(ProcDef& node) {
  admin->debug("proc def");
  access = DEF;
  node.get_id().visit(*this);

  ops.push_back("PROC");
  current_address += 1;

  node.get_block().visit(*this);
  ops.push_back("ENDPROC");
  current_address++;
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
  } else if (access == CALL) {
    TableEntry ent = table_find(name);

    ops.push_back("CALL");
    ops.push_back(to_string(table.size() - 1 - ent.block));
    ops.push_back(to_string(ent.address));
    
  } else {
    TableEntry ent = table_find(name);

    ops.push_back("VARIABLE");
    ops.push_back(to_string(table.size() - 1 - ent.block));
    ops.push_back(to_string(ent.displace));
    current_address += 3;

    if (access == VAL) {
      ops.push_back("VALUE"); 
      current_address++;
    }
  }
}

void CodeGenPL::visit(ConstId& node) {
  string name = node.get_name();
  admin->debug("id = " + name);

  if (access == DEF) {
    // If we are defining a scalar constant just put it in the table
    TableEntry ent;
    ent.address = current_address;
    ent.block = table.size() - 1;
    ent.displace = var_lengths.back() + 3;
    ent.type = node.get_type().type;

    table.back()[name] = ent;  
  } else {
    // for DEF it is an array size
    // In all other situations we just want to treat it as it's value
    node.get_value().visit(*this);
  }
}

void CodeGenPL::visit(Constant& node) {
  // Has a type, value, dec 
  admin->debug("constant " + to_string(access));

  if (access == SIZE) {
    var_lengths.back() += node.get_value();
    return;

  } else if (access == VAR) {
    // Array bounds for array indexing
    ops.push_back( to_string(node.get_value()) );
    return;
  }

  // will need to access type when we do more than ints
  int value = node.get_value();
  ops.push_back("CONSTANT");
  ops.push_back(to_string(value));
  current_address += 2;
}

void CodeGenPL::visit(Access& node) {
  // Has an id 
  admin->debug("access");
  node.get_id().visit(*this);
}

void CodeGenPL::visit(ArrayAccess& node) {
  admin->debug("array access");

  auto acs = access;  // save access type

  access = VAR;
  node.get_id().visit(*this);

  access = VAL;
  node.get_index().visit(*this);

  ops.push_back("INDEX");
  access = SIZE;
  var_lengths.push_back(0);
  node.get_id().get_size().visit(*this);  // To add size for bounds
  ops.push_back( to_string(var_lengths.back()) );
  ops.push_back("-1");  // Supposed to be line number for interpreter error
  var_lengths.pop_back();

  if (acs == VAL) {  // If we are accessing for value
    ops.push_back("VALUE");
    current_address++;
  }
  current_address += 3;
}


void CodeGenPL::visit(Binary& node) {
  // Has an op, lhs, rhs
  admin->debug("binary");
  access = VAL;
  node.get_lhs().visit(*this);
  node.get_rhs().visit(*this);
  ops.push_back(symbol::str(node.get_op().op));
  current_address++;
}


void CodeGenPL::visit(Unary& node) {
  admin->debug("unary");
  node.get_expr().visit(*this);
  ops.push_back(symbol::str(node.get_op().op));
  current_address++;
}



// Stmt nodes
void CodeGenPL::visit(Block& node) {
  // Has Defs and Stmts. Both could be Seq, singular, or empty?
  admin->debug("block");

  var_lengths.push_back(0);
  table.push_back(map<string, TableEntry>());

  int x = current_address++;
  int y = current_address++;
  ops.push_back("???");
  ops.push_back("???");

  node.get_defs().visit(*this);
  ops.at(x) = to_string(var_lengths.back());
  ops.at(y) = to_string(current_address);

  node.get_stmts().visit(*this);

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
  symbol::Tag type = node.get_io_type();

  if (type == symbol::WRITE)
    access = VAL;
  else
    access = VAR;
  node.get_expr().visit(*this);

  ops.push_back(symbol::str(type));
  ops.push_back("1");
  current_address+=2;
}

void CodeGenPL::visit(Asgn& node) {
  // Has an access and expr
  admin->debug("assign");

  access = VAR;
  node.get_acs().visit(*this);

  access = VAL;
  node.get_expr().visit(*this);

  ops.push_back("ASSIGN");
  ops.push_back("1");
  current_address += 2;
}


void CodeGenPL::visit(IfStmt& node) {
  admin->debug("if");

  jumps.push_back(-1);
  node.get_conds().visit(*this);

  // Set all bar values to the proper address
  for (auto it = jumps.rbegin(); it != jumps.rend(); it++) {
    if (*it == -1) {
      jumps.pop_back();
    } else {
      ops.at(*it) = to_string(current_address);
      jumps.pop_back();  // safe because we are going backwards ???
    }
  }
}

void CodeGenPL::visit(Loop& node) {
  admin->debug("loop");
  int start = current_address;

  node.get_cond().visit(*this);
  ops.back() = to_string(start);

  jumps.pop_back();  // Remove the jump address added for condition end
}

void CodeGenPL::visit(Cond& node) {
  // has an cond and stmts
  admin->debug("cond");
  node.get_cond().visit(*this);

  ops.push_back("ARROW");
  ops.push_back("???");

  int arrow = current_address + 1;
  current_address += 2;

  node.get_stmts().visit(*this);

  ops.push_back("BAR");
  ops.push_back("???");

  jumps.push_back(current_address + 1);
  current_address += 2;

  ops.at(arrow) = to_string(current_address);
}


void CodeGenPL::visit(CondSeq& node) {
  // has a first and rest like seq
  admin->debug("cond seq");
  node.get_first().visit(*this);
  node.get_rest().visit(*this);
}


void CodeGenPL::visit(Proc& node) {
  admin->debug("call proc");
  access = CALL;
  node.get_id().visit(*this);
  ops.at(ops.size() - 3) = "CALL";
  current_address += 3;
}


// Helpers ////////////////////////////////////////////////////////////

TableEntry CodeGenPL::table_find(std::string name) {
  for (auto it = table.rbegin(); it != table.rend(); it++) {
    auto entry_it = it->find(name);
    if (entry_it != it->end())
      return entry_it->second;
  }
  throw fatal_error("entry not found in codegen table");
}
