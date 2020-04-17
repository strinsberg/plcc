#include "Actions.h"
#include "Admin.h"
#include "AstStacks.h"
#include "Symbol.h"
#include <string>
#include <iostream>

using namespace std;

Actions::Actions(Admin* a) : admin(a) {};

Actions::~Actions() {
}


// Definitions ///////////////////////////////////////////////////

void Actions::new_token(symbol::Tag tag, string lexeme) {
  if (tag != symbol::NAME)
    stacks.push_token(tag);
  else
    stacks.push_word(lexeme);
}

void Actions::def_part(int num_defs) {
  Def* def = stacks.pop_def();
  for (int i = 0; i < num_defs - 1; i++ ) {
    def = new DefSeq( stacks.pop_def(), def);
  }
  stacks.push_def(def);
}

void Actions::const_def() {
  Expr* value = stacks.pop_expr();
  var_def(symbol::SCALAR, 1);
  delete value;
}

void Actions::array_def(int vars) {
  Expr* size = stacks.pop_expr();
  var_def(symbol::ARRAY, vars);
  delete size;
}

void Actions::var_def(symbol::Tag kind, int vars) {
  Token* type = stacks.get_token(vars);
  add_vars(type->tag, kind, vars);
  type = stacks.pop_token();  // get token has not popped type token yet
  delete type;
}


void Actions::proc_def() {
  stacks.print_tokens();
  stacks.print_nodes();
  Def* name = stacks.pop_def();
  stacks.push_def( new ProcDef(name, stacks.pop_stmt()) ); 
}


void Actions::add_vars(symbol::Tag type, symbol::Tag kind, int vars) {
  Def* def = nullptr;
  for (int i = 0; i < vars; i++) {
    Token* name = stacks.pop_token();
    string lexeme = name->to_string();
    Word* w = new Word(lexeme);

    Id* id = new Id(w, type, kind);
    bool added = table.put(lexeme, id);

    if (!added) {
      admin->error("'" + lexeme + "' already declared");
      delete id;
    } else {
      Def* var = new VarDef(id);
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
  def_part(num_defs);
  stmt_part(num_stmts);
  stacks.push_stmt( new Block(stacks.pop_def(), stacks.pop_stmt()) );
  table.pop_block();
}

void Actions::stmt_part(int num_stmts) {
  Stmt* stmt = stacks.pop_stmt();
  for (int i = 0; i < num_stmts - 1; i++) {
    stmt = new Seq( stacks.pop_stmt(), stmt ); 
  }
  stacks.push_stmt(stmt);
}

void Actions::io(int num_expr, symbol::Tag type) {
  Stmt* stmt = new IoStmt(stacks.pop_expr(), type);
  for (int i = 0; i < num_expr - 1; i++) {  
    stmt = new Seq( new IoStmt(stacks.pop_expr(), type), stmt );
  }
  stacks.push_stmt(stmt);
}

void Actions::assign(int num_vars, int num_exprs) {
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
    Expr* acs = lhs.at(i);
    Expr* expr = rhs.at(i);

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
  Stmt* cond = stacks.pop_stmt();
  for (int i = 0; i < num_cond - 1; i++)
    cond = new Seq(stacks.pop_stmt(), cond);
  stacks.push_stmt( new IfStmt(cond) );
}

void Actions::loop() {
  stacks.push_stmt( new Loop(stacks.pop_stmt()) ); 
}

void Actions::empty() {
  stacks.push_stmt( new Stmt() );
}

void Actions::proc_stmt() {
  // this code should probably be in a function
  Token* name = stacks.pop_token();
  string lexeme = name->to_string();
  Id* id = table.get(lexeme);

  if (id == nullptr) {
    admin->error("'" + lexeme + "' is undeclared");
    stacks.push_stmt( new Stmt() );
    return;
  }

  stacks.push_stmt( new Proc(id) );
}

void Actions::condition(int num_stmts) {
  Expr* cond = stacks.pop_expr();
  stmt_part(num_stmts);
  Stmt* stmt = stacks.pop_stmt();
  stacks.push_stmt( new Cond(cond, stmt) );
}

// Expression methods /////////////////////////////////////////////////

void Actions::access(symbol::Tag type) {
  Token* name = stacks.pop_token();
  string lexeme = name->to_string();
  Id* id = table.get(lexeme);

  if (id == nullptr) {
    admin->error("'" + lexeme + "' is undeclared");
    stacks.push_stmt( new Stmt() );
    return;
  }

  Access* acs = nullptr;
  if (type == symbol::ARRAY) {
    Expr* idx = stacks.pop_expr();
    acs = new ArrayAccess(id, idx);  // Can do type check on index
  } else {
    acs = new Access(id);
  }

  stacks.push_expr(acs);
  delete name;
}

void Actions::binary() {
  Token* op = stacks.pop_token();
  Expr* rhs = stacks.pop_expr();
  Expr* lhs = stacks.pop_expr();

  stacks.push_expr( new Binary(op, lhs, rhs) ); 
}

void Actions::unary(symbol::Tag t) {
  Token* op = new Token(t);
  Expr* expr = stacks.pop_expr();

  stacks.push_expr( new Unary(op, expr) );
}

void Actions::constant(symbol::Tag tag, int val, int dec) {
  Token* tok;
  symbol::Tag type;

  if (tag == symbol::TRUE or tag == symbol::FALSE) {
    tok = new Token(tag);
    type = symbol::BOOL;
  } else if (tag == symbol::INT) {
    tok = new Number(val);
    type = symbol::INT;
  } else if (tag == symbol::FLOAT) {
    tok = new Float(val, dec);
    type = symbol::FLOAT;
  } else if (tag == symbol::CHAR) {
    tok = new Char(val);
    type = symbol::CHAR;
  } else {
    tok = stacks.pop_token();
    type = symbol::NAME;
  }

  stacks.push_expr( new Constant(tok, type) );
}


// Display methods ////////////////////////////////////////////////////

void Actions::display() {
  stacks.print_tokens();
  stacks.print_nodes();

  cout << endl;
  cout << "=== BlockTable Nodes ===" << endl;
  table.print();
}

