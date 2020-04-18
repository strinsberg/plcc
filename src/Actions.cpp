#include "Actions.h"
#include "Admin.h"
#include "AstStacks.h"
#include "Symbol.h"
#include "exceptions.h"
#include <string>
#include <iostream>

using namespace std;

Actions::Actions(Admin* a) : admin(a) {};

Actions::~Actions() {
}

// Type and Op ///////////////////////////////////////////////////
void Actions::new_type(symbol::Tag type) {
  admin->debug("type: " + symbol::str(type));
  Type t;
  t.type = type;
  stacks.set_type(t);
};

void Actions::new_op(symbol::Tag op, symbol::Tag type, symbol::Tag qual) {
  admin->debug("op: " + symbol::str(op));
  Type t(type, symbol::OPERATOR, qual);
  stacks.push_op( Operator(op, t) );
};

void Actions::name(string n) {
  // temp id to hold the name on exprssion stack
  stacks.push_expr( new Id(n, Type(), new Expr(Type())) );
}


// Definitions ///////////////////////////////////////////////////

void Actions::def_part(int num_defs) {
  admin->debug("def part: " + to_string(num_defs)); 
  auto def = stacks.pop_def();
  for (int i = 0; i < num_defs - 1; i++ ) {
    def = new DefSeq( stacks.pop_def(), def);
  }
  stacks.push_def(def);
}

void Actions::const_def() {
  admin->debug("const def");
  auto value = stacks.pop_expr();
  var_def(1, symbol::SCALAR, symbol::CONST, value);
}

void Actions::array_def(int vars) {
  admin->debug("array def: " + to_string(vars)); 
  vector<Expr*> names(vars);
  for (auto & n : names)
    n = stacks.pop_expr();

  Expr* size = stacks.pop_expr();
  // Should be added to a def node. Probably to the standard id.
  if (size->get_type().qual != symbol::CONST)
    admin->error("array size must be a constant");
  if (size->get_type().type != symbol::INT)
    admin->error("array size must be int");

  Type type = stacks.get_type();
  type.kind = symbol::ARRAY;

  Def* def = nullptr;
  for (auto& n : names) {
    Id* id = new Id(n->get_name(), type, size); 
    bool added = table.put(n->get_name(), id);

    if (!added) {
      admin->error("'" + n->get_name() + "' already declared");
      delete id;
    } else {
      auto var = new VarDef(id);
      if (def == nullptr)
        def = var;
      else
        def = new DefSeq(var, def);
    }

    delete n;
  }

  if (def == nullptr)
    def = new Def();
  stacks.push_def(def);
}

// No longer needs a kind as arrays are defined separatedly
void Actions::var_def(int vars, symbol::Tag kind, symbol::Tag qual, Expr* value) {
  admin->debug("var def: " + symbol::str(kind) + " " + to_string(vars)); 
  Type type = stacks.get_type();
  type.kind = kind;
  type.qual = qual;
  add_vars(type, vars, value);
}


void Actions::proc_def() {
  admin->debug("proc def");
  auto name = stacks.pop_def();
  stacks.push_def( new ProcDef(name, stacks.pop_stmt()) ); 
}


void Actions::add_vars(Type type, int vars, Expr* value) {
  admin->debug("add vars: " + symbol::str(type.type) + " " + to_string(vars)); 
  Expr* size = new Constant(
      Type(symbol::INT, symbol::UNIVERSAL, symbol::CONST), 1, 0);

  Def* def = nullptr;
  for (int i = 0; i < vars; i++) {
    auto name = stacks.pop_expr();
    // Should probably also be doing some type checkng for proper sizes
    Id* id = new Id(name->get_name(), type, size);

    if (type.qual == symbol::CONST) {
      try {
        id = new ConstId(name->get_name(), type, value);
      } catch (const exception& e) {
        admin->error("type error: " + string(e.what()), name->get_name());
      }
    }

    // may be possible to move this to it's own function to reduce duplicate code
    // and make future composit defs have easy access to it like array def needs
    // give it a param for id and def and just update the def
    bool added = table.put(name->get_name(), id);
    if (!added) {
      admin->error("'" + name->get_name() + "' already declared");
      delete id;
    } else {
      auto var = new VarDef(id);
      if (def == nullptr)
        def = var;
      else
        def = new DefSeq(var, def);
    }

    delete name;
  }

  if (def == nullptr)
    def = new Def();
  stacks.push_def(def);
}


// Statement methods //////////////////////////////////////////////////

void Actions::block(int num_defs, int num_stmts) {
  admin->debug("block: " + to_string(num_defs) + ", " + to_string(num_stmts));
  def_part(num_defs);
  stmt_part(num_stmts);
  stacks.push_stmt( new Block(stacks.pop_def(), stacks.pop_stmt()) );
  table.pop_block();
}

void Actions::stmt_part(int num_stmts) {
  admin->debug("stmt part: " + to_string(num_stmts));
  auto stmt = stacks.pop_stmt();
  for (int i = 0; i < num_stmts - 1; i++) {
    stmt = new Seq( stacks.pop_stmt(), stmt ); 
  }
  stacks.push_stmt(stmt);
}

void Actions::io(int num_expr, symbol::Tag type) {
  admin->debug("io: " + to_string(num_expr) + " " + symbol::str(type));
  Stmt* stmt = new IoStmt(stacks.pop_expr(), type);
  for (int i = 0; i < num_expr - 1; i++) {  
    stmt = new Seq( new IoStmt(stacks.pop_expr(), type), stmt );
  }
  stacks.push_stmt(stmt);
}

void Actions::assign(int num_vars, int num_exprs) {
  admin->debug("assign: " + to_string(num_vars) + ", " + to_string(num_exprs));
  if (num_vars != num_exprs) {
    admin->error("number of variables does not match number of exressions");
    stacks.push_stmt(new Stmt() );
    return;
  }

  // get all the expressions
  vector<Expr*> rhs(num_vars);
  for (auto & e : rhs) e = stacks.pop_expr();

  vector<Expr*> lhs(num_vars);
  for (auto & e : lhs) e = stacks.pop_expr();

  // pair each access with it's value
  Stmt* stmt = nullptr;
  for (int i = 0; i < num_vars; i++) {
    auto acs = lhs.at(i);
    auto expr = rhs.at(i);

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
  stacks.push_stmt(stmt);
}

void Actions::if_stmt(int num_cond) {
  admin->debug("if: " + to_string(num_cond));
  auto cond = stacks.pop_stmt();
  for (int i = 0; i < num_cond - 1; i++)
    cond = new Seq(stacks.pop_stmt(), cond);
  stacks.push_stmt( new IfStmt(cond) );
}

void Actions::loop() {
  admin->debug("loop");
  stacks.push_stmt( new Loop(stacks.pop_stmt()) ); 
}

void Actions::empty() {
  admin->debug("empty");
  stacks.push_stmt( new Stmt() );
}

void Actions::proc_stmt() {
  admin->debug("call");

  auto id = get_id();
  if (id == nullptr)
    return;

  Stmt* stmt = new Stmt();
  try {
    stmt = new Proc(id);
  } catch (const exception& e) {
    admin->error("type error: " + string(e.what()));
  }

  stacks.push_stmt(stmt);
}

void Actions::condition(int num_stmts) {
  admin->debug("condition: " + to_string(num_stmts));

  auto cond = stacks.pop_expr();
  stmt_part(num_stmts);
  auto stmt = stacks.pop_stmt();

  Stmt* c = new Stmt();
  try {
    c = new Cond(cond, stmt);
  } catch (const exception& e) {
    admin->error("type error: " + string(e.what()) +
                 ". actual type: " + symbol::str(cond->get_type().type));
  }

  stacks.push_stmt(c);
}

// Expression methods /////////////////////////////////////////////////

void Actions::access(symbol::Tag kind) {
  admin->debug("access: " + symbol::str(kind));

  Expr* idx;
  if (kind == symbol::ARRAY)
    idx = stacks.pop_expr();

  auto id = get_id();
  if (id == nullptr)
    return;

  Expr* acs = new Expr(Type());
  try {
    if (kind == symbol::ARRAY) {
      acs = new ArrayAccess(id, idx);
    } else {
      acs = new Access(id);
    }
  } catch (const exception& e) {
    admin->error("type error: " + string(e.what()), id->get_name());
  }

  stacks.push_expr(acs);
}

void Actions::binary() {
  admin->debug("binary");
  Operator op = stacks.pop_op();
  auto rhs = stacks.pop_expr();
  auto lhs = stacks.pop_expr();

  auto bin = new Expr(Type());
  try {
    bin = new Binary(op, lhs, rhs);
  } catch (const type_error& e) {
    admin->error("type error: " + string(e.what()), symbol::str(op.op));
  }

  stacks.push_expr(bin); 
}

void Actions::unary() {
  admin->debug("unary");
  Operator op = stacks.pop_op();
  auto expr = stacks.pop_expr();

  auto un = new Expr(Type());
  try {
    un = new Unary(op, expr);
  } catch (const type_error& e) {
    admin->error("type error: " + string(e.what()), symbol::str(op.op));
  }

  stacks.push_expr(un);
}

void Actions::constant(symbol::Tag tag, int val, double dec) {
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
    access(symbol::CONST);
    // need to check for const here or in access somehow
    return;
  }

  stacks.push_expr(con);
}


// Display methods ////////////////////////////////////////////////////

void Actions::display() {
  stacks.print_nodes();

  cout << endl;
  cout << "=== BlockTable Nodes ===" << endl;
  table.print();
}


// Helpers ////////////////////////////////////////////////////////////
Id* Actions::get_id() {
  auto name = stacks.pop_expr();
  auto id = table.get(name->get_name());

  if (id == nullptr) {
    admin->error("'" + name->get_name() + "' is undeclared");
    stacks.push_stmt( new Stmt() );
  }

  delete name;
  return id;
}
