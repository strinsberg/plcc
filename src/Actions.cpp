#include "Actions.h"
#include "Admin.h"
#include "AstStacks.h"
#include "Symbol.h"
#include "Types.h"
#include "exceptions.h"
#include <string>
#include <iostream>
using namespace std;


Actions::Actions(Admin* a) : admin(a) {};

Actions::~Actions() {}


// Type and Op ///////////////////////////////////////////////////

Type* Actions::new_type(symbol::Tag type) {
  admin->debug("type: " + symbol::str(type));
  Type* t = new Type();
  t->type = type;
  return t;
};

Operator* Actions::new_op(symbol::Tag op, symbol::Tag type, symbol::Tag qual) {
  admin->debug("op: " + symbol::str(op));
  Type t(type, symbol::OPERATOR, qual);
  return new Operator(op, t);
};


// Definitions ///////////////////////////////////////////////////

Def* Actions::def_part(Def* rest, Def* last) {
  admin->debug("def part");
  // combine a rest and last into a seq.
  return rest;
}


Def* Actions::const_def(Type* type, std::string* name, Expr* value) {
  admin->debug("const def");
  type->qual = symbol::CONST;

  auto def = new Def();
  try {
    auto id = new ConstId(*name, *type, value);
    bool added = table.put(*name, id);

    if (!added)
      admin->error("'" + *name + "' already declared");
    else
      def = new VarDef(id);

  } catch (const type_error& e) {
    admin->error("type error: " + string(e.what()), *name);
  }
  return def;
}


Def* Actions::var_def(Type* type, std::pair<Expr*, std::vector<std::string*>*>* pp) {
  admin->debug("var def");

  type->kind = symbol::SCALAR;
  Expr* size = pp->first;
  vector<string*>* names = pp->second;


  return new Def();
}


Def* Actions::array_def(int vars) {
  admin->debug("array def: " + to_string(vars)); 

  vector<Expr*> names(vars);
  for (auto & n : names)
    n = stacks.pop_expr();

  Expr* size = stacks.pop_expr();
  Type type = stacks.get_type();
  type.kind = symbol::ARRAY;

  return new Def();
}


Def* Actions::proc_def(string* name, Stmt* block) {
  admin->debug("proc def");
  // this will again not declare the name before the block is resolved
  // so the rules will be adjusted again
  return new ProcDef(new Expr(Type()), block); 
}


// private def helpers //
Def* Actions::add_vars(vector<string*>* names, Type* type, Expr* size) {
  admin->debug("add_vars");

  Def* def = nullptr;
  for (auto& n : *names) {
    try {
      Id* id = new Id(*n, *type, size); 
      bool added = table.put(*n, id);

      if (!added) {
        admin->error("'" + *n + "' already declared");
        delete id;
      } else {
        auto var = new VarDef(id);
        if (def == nullptr)
          def = var;
        else
          def = new DefSeq(var, def);
      }
    } catch (const type_error& e) {
      admin->error("type error: " + string(e.what()), *n);
    }
  }

  if (def == nullptr)
    def = new Def();

  return def;
}

std::pair<Expr*, std::vector<std::string*>*>* Actions::vprime(
    std::vector<std::string*>* rest, Expr* e) {
  // obviously just to get it compiling for now
  return new pair<Expr*, vector<string*>*>{e, rest};
}

std::vector<std::string*>* Actions::var_list(
    std::vector<std::string*>* rest, std::string* last) {
  return rest;
}

// Statement methods //////////////////////////////////////////////////

Stmt* Actions::block(Def* defs, Stmt* stmts) {
  admin->debug("block");
  table.pop_block();
  return new Block(defs, stmts);
}


Stmt* Actions::stmt_part(Stmt* rest, Stmt* last) {
  admin->debug("stmt part");
  // create a seq of stmts from these two
  return rest;
}


Stmt* Actions::io(std::vector<Expr*>* exprs, symbol::Tag type) {
  admin->debug("io");
  Stmt* stmt = new IoStmt(exprs->at(0), type);
  for (size_t i = 1; i < exprs->size(); i++) {  
    stmt = new Seq( new IoStmt(exprs->at(i), type), stmt );
  }
  return stmt;
}


Stmt* Actions::assign(vector<Expr*>* vars, vector<Expr*>* values) {
  admin->debug("assign");
  if (vars->size() != values->size()) {
    admin->error("number of variables does not match number of exressions");
    return new Stmt();
  }

  // pair each access with it's value
  Stmt* stmt = nullptr;
  for (size_t i = 0; i < vars->size(); i++) {
    auto acs = vars->at(i);
    auto expr = values->at(i);

    Stmt* asgn = new Stmt();
    try {
      asgn = new Asgn(acs, expr);
    } catch (const exception& e) {
      admin->error("type error: " + string(e.what()), acs->get_name());
    }

    if (stmt == nullptr)
      stmt = asgn;
    else
      stmt = new Seq(asgn, stmt);
  }

  if (stmt == nullptr)
    stmt = new Stmt();
  return stmt;
}


Stmt* Actions::if_stmt(Stmt* cond) {
  admin->debug("if");
  return new IfStmt(cond);
}


Stmt* Actions::loop(Stmt* cond) {
  admin->debug("loop");
  return new Loop(cond);
}


Stmt* Actions::empty_stmt() {
  admin->debug("empty");
  return new Stmt();
}


Stmt* Actions::proc_stmt(std::string* name) {
  admin->debug("call");

  auto id = get_id(*name);
  if (id == nullptr)
    return new Stmt();

  Stmt* stmt = new Stmt();
  try {
    stmt = new Proc(id);
  } catch (const exception& e) {
    admin->error("type error: " + string(e.what()), *name);
  }

  return stmt;
}

Stmt* Actions::conditions(Stmt* rest, Stmt* last) {
  // like stmt part combine these into a seq
  return rest;
}

Stmt* Actions::condition(Expr* expr, Stmt* stmts) {
  admin->debug("condition");

  Stmt* cond = new Stmt();
  try {
    cond = new Cond(expr, stmts);
  } catch (const exception& e) {
    admin->error("type error: " + string(e.what()) +
                 ". actual type: " + symbol::str(expr->get_type().type));
  }

  return cond;
}


// Expression methods /////////////////////////////////////////////////

std::vector<Expr*>* Actions::expr_list(std::vector<Expr*>* rest, Expr* last) {
  return rest;
}

Expr* Actions::access(string* name, Expr* idx) {
  admin->debug("access");

  auto id = get_id(*name);
  if (id == nullptr)
    return new Expr(Type());

  Expr* acs = new Expr(Type());
  try {
    if (idx->get_type().type != symbol::EMPTY) {
      acs = new ArrayAccess(id, idx);
    } else {
      acs = new Access(id);
    }
  } catch (const exception& e) {
    admin->error("type error: " + string(e.what()), id->get_name());
  }

  return acs;
}


Expr* Actions::binary(Operator* op, Expr* lhs, Expr* rhs) {
  admin->debug("binary");

  auto bin = new Expr(Type());
  try {
    bin = new Binary(*op, lhs, rhs);
  } catch (const type_error& e) {
    admin->error("type error: " + string(e.what()), symbol::str(op->op));
  }

  return bin; 
}


Expr* Actions::unary(symbol::Tag op_type, Expr* expr) {
  admin->debug("unary");
  Operator* op;
  if (op_type == symbol::MINUS)
    op = new_op(symbol::MINUS, symbol::NUMBER, symbol::NUMBER);
  else
    op = new_op(symbol::NOT, symbol::BOOL, symbol::BOOL);

  auto un = new Expr(Type());
  try {
    un = new Unary(*op, expr);
  } catch (const type_error& e) {
    admin->error("type error: " + string(e.what()), symbol::str(op->op));
  }

  return un;
}


Expr* Actions::constant(symbol::Tag tag, int val, double dec) {
  admin->debug("constant: " + symbol::str(tag) + " " + to_string(val)); 
  Expr* con;
  Type t;
  t.type = tag;

  if (tag == symbol::TRUE or tag == symbol::FALSE) {
    t.type = symbol::BOOL;
    t.qual = symbol::CONST;
    con = new Constant(t, val, dec);

  } else if (tag == symbol::INT) {
    t.qual = symbol::CONST;
    con = new Constant(t, val, dec);

  } else if (tag == symbol::FLOAT) {
    t.qual = symbol::CONST;
    int size = to_string((int)dec).size();
    for (int i = 0; i < size; i++)
      dec /= 10;
    con = new Constant(t, 0, val + dec);

  } else if (tag == symbol::CHAR) {
    t.qual = symbol::CONST;
    con = new Constant(t, val, dec);
  } else {
    admin->error("not a valid constant type: " + symbol::str(tag));
    con = new Expr(Type());
  }

  return con;
}


// Display methods ////////////////////////////////////////////////////

void Actions::display() {
  stacks.print_nodes();

  cout << endl;
  cout << "=== BlockTable Nodes ===" << endl;
  table.print();
}


// Helpers ////////////////////////////////////////////////////////////
Id* Actions::get_id(string name) {
  auto id = table.get(name);

  if (id == nullptr) {
    admin->error("'" + name + "' is undeclared");
    stacks.push_stmt( new Stmt() );
  }

  return id;
}
