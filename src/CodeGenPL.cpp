#include "CodeGenPL.h"
#include "TreeWalker.h"
#include "AstNode.h"
#include "Defs.h"
#include "Exprs.h"
#include "Stmts.h"
#include "Admin.h"
#include "exceptions.h"
#include "Interpreter.h"
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
  ops.push_back(symbol::OP_PROG);
  current_address += 1;
  node.visit(*this);
  ops.push_back(symbol::OP_ENDPROG);

  for (auto & op : ops)
    *out << op << endl;
}

void CodeGenPL::visit(AstNode& node) {
  admin->debug("ASTNODE VISIT");
}


// Def nodes
void CodeGenPL::visit(DefSeq& node) {
  admin->debug("def seq");
  node.get_first().visit(*this);
  node.get_rest().visit(*this);
}

void CodeGenPL::visit(VarDef& node) {
  admin->debug("var def");
  access = DEF;
  node.get_id().visit(*this);

  if (node.get_id().get_type().qual != symbol::CONST) {
    access = SIZE;
    node.get_id().get_size().visit(*this);
  }
}


void CodeGenPL::visit(ProcDef& node) {
  admin->debug("proc def");
  access = DEF;
  node.get_id().visit(*this);

  ops.push_back(symbol::OP_PROC);
  current_address += 1;

  node.get_block().visit(*this);
  ops.push_back(symbol::OP_ENDPROC);
  current_address++;
}


// Expr nodes
void CodeGenPL::visit(Id& node) {
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

    ops.push_back(symbol::OP_CALL);
    ops.push_back(table.size() - 1 - ent.block);
    ops.push_back(ent.address);
    
  } else if (access == SIZE) {
    node.get_size().visit(*this);

  } else {
    TableEntry ent = table_find(name);

    ops.push_back(symbol::OP_VARIABLE);
    ops.push_back(table.size() - 1 - ent.block);
    ops.push_back(ent.displace);
    current_address += 3;

    if (access == VAL) {
      symbol::OpCode code = symbol::to_op(ent.type);
      ops.push_back(code); 
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
    // In all other situations we just want to treat it as it's value
    node.get_value().visit(*this);
  }
}

void CodeGenPL::visit(Constant& node) {
  admin->debug("constant " + to_string(access));

  if (access == SIZE) {
    var_lengths.back() += node.get_value();
    return;

  } else if (access == VAR) {
    // Array bounds for array indexing
    ops.push_back(node.get_value());
    return;
  }

  // Push op type, value(s), and type of the constant
  int value = node.get_value();
  int exp = node.get_exp();
  symbol::Tag type = node.get_type().type;

  if (type == symbol::FLOAT) {
    ops.push_back(symbol::OP_DB_CONSTANT);
    ops.push_back(value);
    ops.push_back(exp);
  } else {
    ops.push_back(symbol::OP_CONSTANT);
    ops.push_back(value);
  }

  ops.push_back(symbol::to_op(type)); 
  current_address += 3;
}


void CodeGenPL::visit(ConstString& node) {
  string& str = node.get_string();
  admin->debug("constant string " + str);

  if (access == VAL) {
    for (auto& c : str) {
      ops.push_back(symbol::OP_CONSTANT);
      ops.push_back(c);
      ops.push_back(symbol::OP_CHAR);
      current_address += 3;
    }
  } else if (access == SIZE) {
    var_lengths.back() = node.get_value();
  }
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

  ops.push_back(symbol::OP_INDEX);
  access = SIZE;
  var_lengths.push_back(0);
  node.get_id().get_size().visit(*this);  // To add size for bounds
  ops.push_back(var_lengths.back());
  ops.push_back(-2);  // Supposed to be line number for interpreter error
  var_lengths.pop_back();

  if (acs == VAL or acs == SIZE) {  // size is for write array access
    ops.push_back(symbol::to_op(node.get_id().get_type().type));
    current_address++;
  }
  current_address += 3;
}


void CodeGenPL::visit(Binary& node) {
  admin->debug("binary");
  access = VAL;
  node.get_lhs().visit(*this);
  access = VAL;
  node.get_rhs().visit(*this);

  symbol::Tag op = node.get_op().op;
  bool sub = op == symbol::MINUS;
  symbol::OpCode code = symbol::to_op(op, sub);

  ops.push_back(code);
  current_address++;
}


void CodeGenPL::visit(Unary& node) {
  admin->debug("unary");
  access = VAL;
  node.get_expr().visit(*this);

  symbol::OpCode code = symbol::to_op(node.get_op().op);
  ops.push_back(code);
  current_address++;
}



// Stmt nodes
void CodeGenPL::visit(BlockStmt& node) {
  admin->debug("block stmt");
  ops.push_back(symbol::OP_BLOCK);
  current_address++;
  node.get_block().visit(*this);
  ops.push_back(symbol::OP_ENDBLOCK);
  current_address++;
}

void CodeGenPL::visit(Block& node) {
  admin->debug("block");

  var_lengths.push_back(0);
  table.push_back(map<string, TableEntry>());

  int x = current_address++;
  int y = current_address++;
  ops.push_back(-2);
  ops.push_back(-2);

  node.get_defs().visit(*this);
  ops.at(x) = var_lengths.back();
  ops.at(y) = current_address;

  node.get_stmts().visit(*this);

  table.pop_back();
  var_lengths.pop_back();
}

void CodeGenPL::visit(Seq& node) {
  admin->debug("seq");
  node.get_first().visit(*this);
  node.get_rest().visit(*this);
}

void CodeGenPL::visit(IoStmt& node) {
  admin->debug("io");

  int size = 1;
  symbol::Tag op_type = node.get_io_type();
  Type e_type = node.get_expr().get_type();

  if (op_type == symbol::WRITE) {

    if (e_type.kind == symbol::ARRAY) {
      access = SIZE;
      var_lengths.push_back(0);
      node.get_expr().visit(*this);
      size = var_lengths.back();
      var_lengths.pop_back();
      cout << "========== " << symbol::str(e_type.type) << ", " << size << endl;

      if (e_type.type == symbol::STRING) {
        access = VAL;
        node.get_expr().visit(*this);

      } else {
        access = VAR;
        for (int i = 0; i < size; i++) {
          node.get_expr().visit(*this);
          ops.push_back(symbol::OP_CONSTANT);
          ops.push_back(i);
          ops.push_back(symbol::OP_INT);
        
          ops.push_back(symbol::OP_INDEX);
          ops.push_back(size);
          ops.push_back(-2);  // Supposed to be line number for interpreter error
          ops.push_back(symbol::to_op(e_type.type));
          current_address += 7;
        }
      }

    } else {
      access = VAL;
      node.get_expr().visit(*this);
    }

  } else {
    access = VAR;
    node.get_expr().visit(*this);
  }

  if (size == 0)
    size++;
  symbol::OpCode code = symbol::to_op(op_type);
  ops.push_back(code);
  ops.push_back(size);
  current_address += 2;

  if (op_type == symbol::READ) {
    ops.push_back( symbol::to_op(e_type.type) );
    current_address++;
  }
}

void CodeGenPL::visit(Asgn& node) {
  // Has an access and expr
  admin->debug("assign");

  access = VAR;
  node.get_acs().visit(*this);

  access = VAL;
  node.get_expr().visit(*this);

  ops.push_back(symbol::OP_ASSIGN);
  ops.push_back(1);
  current_address += 2;
}

void CodeGenPL::visit(StringAsgn& node) {
  admin->debug("string assign");

  access = SIZE;
  var_lengths.push_back(0);
  node.get_str().visit(*this);
  int size = var_lengths.back();
  var_lengths.pop_back();

  access = VAR;
  for (int i = 0; i < size; i++) {
    node.get_acs().visit(*this);
    ops.push_back(symbol::OP_CONSTANT);
    ops.push_back(i);
    ops.push_back(symbol::OP_INT);
  
    ops.push_back(symbol::OP_INDEX);
    // Should check to make sure string assignment string fits in array during parsing
    ops.push_back(size);
    ops.push_back(-2);  // Supposed to be line number for interpreter error
  }

  access = VAL;
  node.get_str().visit(*this);

  ops.push_back(symbol::OP_ASSIGN);
  ops.push_back(size);
}

void CodeGenPL::visit(IfStmt& node) {
  admin->debug("if");

  jumps.push_back(-2);
  node.get_conds().visit(*this);

  // Set all bar values to the proper address
  for (auto it = jumps.rbegin(); it != jumps.rend(); it++) {
    if (*it == -2) {
      jumps.pop_back();
    } else {
      ops.at(*it) = current_address;
      jumps.pop_back();  // safe because we are going backwards ???
    }
  }
}

void CodeGenPL::visit(Loop& node) {
  admin->debug("loop");
  int start = current_address;

  node.get_cond().visit(*this);
  ops.back() = start;

  jumps.pop_back();  // Remove the jump address added for condition end
}

void CodeGenPL::visit(Cond& node) {
  admin->debug("cond");
  access = VAL;
  node.get_cond().visit(*this);

  ops.push_back(symbol::OP_ARROW);
  ops.push_back(-2);

  int arrow = current_address + 1;
  current_address += 2;

  node.get_stmts().visit(*this);

  ops.push_back(symbol::OP_BAR);
  ops.push_back(-2);

  jumps.push_back(current_address + 1);
  current_address += 2;

  ops.at(arrow) = current_address;
}


void CodeGenPL::visit(CondSeq& node) {
  admin->debug("cond seq");
  node.get_first().visit(*this);
  node.get_rest().visit(*this);
}


void CodeGenPL::visit(Proc& node) {
  admin->debug("call proc");
  access = CALL;
  node.get_id().visit(*this);
  ops.at(ops.size() - 3) = symbol::OP_CALL;
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
