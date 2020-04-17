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

void Actions::new_op(symbol::Tag op, symbol::Tag type) {
  admin->debug("op: " + symbol::str(op));
  Type t;
  t.type = type;
  stacks.set_op( Operator(op, t) );
};

// Definitions ///////////////////////////////////////////////////

void Actions::new_token(symbol::Tag tag, string lexeme) {
  if (tag != symbol::NAME)
    stacks.push_token(tag);
  else
    stacks.push_word(lexeme);
}

void Actions::def_part(int num_defs) {
  admin->debug("def part: " + std::to_string(num_defs)); 
  auto def = stacks.pop_def();
  for (int i = 0; i < num_defs - 1; i++ ) {
    def = new DefSeq( stacks.pop_def(), def);
  }
  stacks.push_def(def);
}

void Actions::const_def() {
  admin->debug("const def");
  auto value = stacks.pop_expr();  // Still unused?
  var_def(symbol::SCALAR, 1);  // perhaps chang this later if other types can be const
  delete value;
}

void Actions::array_def(int vars) {
  admin->debug("array def: " + std::to_string(vars)); 
  auto size = stacks.pop_expr();  // Still unused?
  var_def(symbol::ARRAY, vars);
  delete size;
}

void Actions::var_def(symbol::Tag kind, int vars) {
  admin->debug("var def: " + symbol::to_string.at(kind) + " " + std::to_string(vars)); 
  Type type = stacks.get_type();
  add_vars(type, kind, vars);
}


void Actions::proc_def() {
  admin->debug("proc def");
  auto name = stacks.pop_def();
  stacks.push_def( new ProcDef(name, stacks.pop_stmt()) ); 
}


void Actions::add_vars(Type type, symbol::Tag kind, int vars) {
  admin->debug("add vars: " + symbol::str(type.type) + " " + symbol::to_string.at(kind) + " " + std::to_string(vars)); 
  type.kind = kind;

  Def* def = nullptr;
  for (int i = 0; i < vars; i++) {
    Token* name = stacks.pop_token();
    string lexeme = name->to_string();
    Word* w = new Word(lexeme);

    auto id = new Id(w, type);
    bool added = table.put(lexeme, id);

    if (!added) {
      admin->error("'" + lexeme + "' already declared");
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
  admin->debug("block: " + std::to_string(num_defs) + ", " + std::to_string(num_stmts));
  def_part(num_defs);
  stmt_part(num_stmts);
  stacks.push_stmt( new Block(stacks.pop_def(), stacks.pop_stmt()) );
  table.pop_block();
}

void Actions::stmt_part(int num_stmts) {
  admin->debug("stmt part: " + std::to_string(num_stmts));
  auto stmt = stacks.pop_stmt();
  for (int i = 0; i < num_stmts - 1; i++) {
    stmt = new Seq( stacks.pop_stmt(), stmt ); 
  }
  stacks.push_stmt(stmt);
}

void Actions::io(int num_expr, symbol::Tag type) {
  admin->debug("io: " + std::to_string(num_expr) + " " + symbol::to_string.at(type));
  Stmt* stmt = new IoStmt(stacks.pop_expr(), type);
  for (int i = 0; i < num_expr - 1; i++) {  
    stmt = new Seq( new IoStmt(stacks.pop_expr(), type), stmt );
  }
  stacks.push_stmt(stmt);
}

void Actions::assign(int num_vars, int num_exprs) {
  admin->debug("assign: " + std::to_string(num_vars) + ", " + std::to_string(num_exprs));
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

    Asgn* asgn = new Asgn(acs, expr);  // will do type checking
    if (stmt == nullptr)
      stmt = asgn;
    else
      stmt = new Seq(asgn, stmt);  // will do type checking
  }

  if (stmt == nullptr)
    stmt = new Stmt();
  stacks.push_stmt(stmt);
}

void Actions::if_stmt(int num_cond) {
  admin->debug("if: " + std::to_string(num_cond));
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
  // this code should probably be in a function
  Token* name = stacks.pop_token();
  string lexeme = name->to_string();
  auto id = table.get(lexeme);

  if (id == nullptr) {
    admin->error("'" + lexeme + "' is undeclared");
    stacks.push_stmt( new Stmt() );
    return;
  }

  stacks.push_stmt( new Proc(id) );
}

void Actions::condition(int num_stmts) {
  admin->debug("condition: " + std::to_string(num_stmts));
  auto cond = stacks.pop_expr();
  stmt_part(num_stmts);
  auto stmt = stacks.pop_stmt();
  stacks.push_stmt( new Cond(cond, stmt) );
}

// Expression methods /////////////////////////////////////////////////

void Actions::access(symbol::Tag kind) {
  admin->debug("access: " + symbol::to_string.at(kind));
  Token* name = stacks.pop_token();
  string lexeme = name->to_string();
  auto id = table.get(lexeme);

  if (id == nullptr) {
    admin->error("'" + lexeme + "' is undeclared");
    stacks.push_stmt( new Stmt() );
    return;
  }

  Access* acs = nullptr;
  if (kind == symbol::ARRAY) {
    auto idx = stacks.pop_expr();
    acs = new ArrayAccess(id, idx);  // Can do type check on index
  } else {
    acs = new Access(id);
  }

  // Check the kind of access here to make sure that array access is
  // not done on a scalar id. Possibly even add a kind field to access.

  stacks.push_expr(acs);
  delete name;
}

void Actions::binary() {
  admin->debug("binary");
  Operator op = stacks.get_op();
  auto rhs = stacks.pop_expr();
  auto lhs = stacks.pop_expr();

  auto bin = new Expr(Type());
  try {
    bin = new Binary(op, lhs, rhs);
  } catch ( const type_error & e ) {
    admin->error("type error: " + string(e.what()), symbol::str(op.op));
  }

  stacks.push_expr( bin ); 
}

void Actions::unary() {
  admin->debug("unary");
  Operator op = stacks.get_op();
  auto expr = stacks.pop_expr();

  // Catch a type error for using mismatched operators and expressions
  stacks.push_expr( new Unary(op, expr) );
}

void Actions::constant(symbol::Tag tag, int val, double dec) {
  admin->debug("constant: " + symbol::to_string.at(tag) + " " + std::to_string(val)); 
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
  stacks.print_tokens();
  stacks.print_nodes();

  cout << endl;
  cout << "=== BlockTable Nodes ===" << endl;
  table.print();
}

