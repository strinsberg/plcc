#include "Actions.h"
#include "Admin.h"
#include "AstStacks.h"
#include "Symbol.h"
#include <string>
#include <iostream>

using namespace std;

Actions::Actions(Admin* a) : admin(a), line_num(1), has_errors(false) {};

Actions::~Actions() {
}


// Definitions ///////////////////////////////////////////////////

void Actions::new_token(tag::Tag tag, string lexeme) {
  if (tag != tag::NAME)
    stacks.push_token(tag);
  else
    stacks.push_word(lexeme);
}

void Actions::def_part(int num_defs) {
  Def* def = stacks.pop_def();
  for (int i = 0; i < num_defs - 1; i++ ) {
    def = new DefSeq( stacks.pop_def(), def, line_num);
  }
  stacks.push_def(def);
}

void Actions::const_def() {
  Expr* value = stacks.pop_expr();
  var_def(tag::SCALAR, 1);
  delete value;
}

void Actions::array_def(int vars) {
  Expr* size = stacks.pop_expr();
  var_def(tag::ARRAY, vars);
  delete size;
}

void Actions::var_def(tag::Tag kind, int vars) {
  Token* type = stacks.get_token(vars);
  add_vars(type->tag, kind, vars);
  type = stacks.pop_token();  // get token did not pop type token yet
  delete type;
}

void Actions::add_vars(tag::Tag type, tag::Tag kind, int vars) {
  Def* def = nullptr;
  for (int i = 0; i < vars; i++) {
    Token* name = stacks.pop_token();
    string lexeme = name->to_string();
    Word* w = new Word(lexeme);

    Id* id = new Id(w, type, kind, line_num);
    bool added = table.put(lexeme, id);

    if (!added) {
      admin->error("'" + lexeme + "' already declared");
      delete id;
    } else {
      Def* var = new VarDef(id, line_num);
      if (def == nullptr)
        def = var;
      else
        def = new DefSeq( var, def, line_num );
    }

    delete name;
  }

  if (def == nullptr)
    def = new Def(line_num);
  stacks.push_def(def);
}


// Statement methods //////////////////////////////////////////////////

void Actions::block(int num_defs, int num_stmts) {
  def_part(num_defs);
  stmt_part(num_stmts);
  stacks.push_stmt( new Block(stacks.pop_def(), stacks.pop_stmt(), line_num) );
  table.pop_block();
}

void Actions::stmt_part(int num_stmts) {
  Stmt* stmt = stacks.pop_stmt();
  for (int i = 0; i < num_stmts - 1; i++) {
    stmt = new Seq( stacks.pop_stmt(), stmt, line_num ); 
  }
  stacks.push_stmt(stmt);
}

void Actions::write(int num_expr) {
  Stmt* stmt = new Write(stacks.pop_expr(), line_num);
  for (int i = 0; i < num_expr - 1; i++) {  
    stmt = new Seq( new Write(stacks.pop_expr(), line_num), stmt, line_num );
  }
  stacks.push_stmt(stmt);
}

void Actions::assign(int num_vars, int num_exprs) {
  if (num_vars != num_exprs) {
    admin->error("number of variables does not match number of exressions");
    stacks.push_stmt(new Stmt(line_num) );
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

    Asgn* asgn = new Asgn(acs, expr, line_num);  // will do type checking
    if (stmt == nullptr)
      stmt = asgn;
    else
      stmt = new Seq(asgn, stmt, line_num);  // will do type checking
  }

  if (stmt == nullptr)
    stmt = new Stmt(line_num);
  stacks.push_stmt(stmt);
}

void Actions::if_stmt(int num_cond) {
  Stmt* cond = stacks.pop_stmt();
  for (int i = 0; i < num_cond - 1; i++)
    cond = new Seq(stacks.pop_stmt(), cond, line_num);
  stacks.push_stmt( new IfStmt(cond , line_num) );
}

void Actions::loop() {
  stacks.push_stmt( new Loop(stacks.pop_stmt(), line_num) ); 
}

void Actions::empty() {
  stacks.push_stmt( new Stmt(line_num) );
}

void Actions::condition(int num_stmts) {
  Expr* cond = stacks.pop_expr();
  stmt_part(num_stmts);
  Stmt* stmt = stacks.pop_stmt();
  stacks.push_stmt( new Cond(cond, stmt, line_num) );
}

// Expression methods /////////////////////////////////////////////////

void Actions::access(tag::Tag type) {
  Token* name = stacks.pop_token();
  string lexeme = name->to_string();
  Id* id = table.get(lexeme);

  if (id == nullptr) {
    admin->error("'" + lexeme + "' is undeclared");
    stacks.push_stmt( new Stmt(line_num) );
    return;
  }

  Access* acs = nullptr;
  if (type == tag::ARRAY) {
    Expr* idx = stacks.pop_expr();
    acs = new ArrayAccess(id, idx, line_num);  // Can do type check on index
  } else {
    acs = new Access(id, line_num);
  }

  stacks.push_expr(acs);
  delete name;
}

void Actions::binary() {
  Token* op = stacks.pop_token();
  Expr* lhs = stacks.pop_expr();
  Expr* rhs = stacks.pop_expr();

  stacks.push_expr( new Binary(op, lhs, rhs, line_num) ); 
}

void Actions::unary(tag::Tag t) {
  Token* op = new Token(t);
  Expr* expr = stacks.pop_expr();

  stacks.push_expr( new Unary(op, expr, line_num) );
}

void Actions::constant(tag::Tag tag, int val, int dec) {
  Token* tok;
  tag::Tag type;

  if (tag == tag::TRUE or tag == tag::FALSE) {
    tok = new Token(tag);
    type = tag::BOOL;
  } else if (tag == tag::INT) {
    tok = new Number(val);
    type = tag::INT;
  } else if (tag == tag::FLOAT) {
    tok = new Float(val, dec);
    type = tag::FLOAT;
  } else if (tag == tag::CHAR) {
    tok = new Char(val);
    type = tag::CHAR;
  } else {
    tok = stacks.pop_token();
    type = tag::NAME;
  }

  stacks.push_expr( new Constant(tok, type, line_num) );
}


// Display methods ////////////////////////////////////////////////////

void Actions::display() {
  stacks.print_tokens();
  stacks.print_nodes();

  cout << endl;
  cout << "=== BlockTable Nodes ===" << endl;
  table.print();
}

