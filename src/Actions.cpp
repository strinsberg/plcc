#include "Actions.h"
#include "AstStacks.h"
#include "Symbol.h"
#include <string>
#include <iostream>

using namespace std;

Actions::Actions() {};

Actions::~Actions() {
}


void Actions::error(string text, int line, string lexeme) {
  cerr << "error on line " << line;
  if (lexeme != "")
    cerr << " near '" << lexeme << "'";
  cerr << ": " << text << endl;
}




// Definitions ///////////////////////////////////////////////////

void Actions::def_part(int num_defs, int line) {
  Def* def = stacks.pop_def();
  for (int i = 0; i < num_defs - 1; i++ ) {
    def = new DefSeq( stacks.pop_def(), def, line);
  }
  stacks.push_def(def);
}

void Actions::const_def(int line) {
  Expr* value = stacks.pop_expr();
  var_def(tag::SCALAR, 1, line);
  delete value;
}

void Actions::array_def(int vars, int line) {
  Expr* size = stacks.pop_expr();
  var_def(tag::ARRAY, vars, line);
  delete size;
}

void Actions::var_def(tag::Tag kind, int vars, int line) {
  Token* type = stacks.get_token(vars);
  add_vars(type->tag, kind, vars, line);
  type = stacks.pop_token();  // get token did not pop type token yet
  delete type;
}

void Actions::add_vars(tag::Tag type, tag::Tag kind, int vars, int line) {
  Def* def = nullptr;
  for (int i = 0; i < vars; i++) {
    Token* name = stacks.pop_token();
    string lexeme = name->to_string();
    Word* w = new Word(lexeme);

    Id* id = new Id(w, type, kind, line);
    bool added = table.put(lexeme, id);

    if (!added) {
      error("'" + lexeme + "' already declared", line);
      delete id;
    } else {
      Def* var = new VarDef(id, line);
      if (def == nullptr)
        def = var;
      else
        def = new DefSeq( var, def, line );
    }

    delete name;
  }

  if (def == nullptr)
    def = new Def(6000);
  stacks.push_def(def);
}


// Statement methods //////////////////////////////////////////////////

void Actions::block(int num_defs, int num_stmts, int line) {
  def_part(num_defs, line);
  stmt_part(num_stmts, line);
  stacks.push_stmt( new Block(stacks.pop_def(), stacks.pop_stmt(), line) );
  table.pop_block();
}

void Actions::stmt_part(int num_stmts, int line) {
  Stmt* stmt = stacks.pop_stmt();
  for (int i = 0; i < num_stmts - 1; i++) {
    stmt = new Seq( stacks.pop_stmt(), stmt, line ); 
  }
  stacks.push_stmt(stmt);
}

void Actions::write(int num_expr, int line) {
  Stmt* stmt = new Write(stacks.pop_expr(), line);
  for (int i = 0; i < num_expr - 1; i++) {  
    stmt = new Seq( new Write(stacks.pop_expr(), line), stmt, line );
  }
  stacks.push_stmt(stmt);
}

void Actions::assign(int num_vars, int num_exprs, int line) {
  if (num_vars != num_exprs) {
    error("number of variables does not match number of exressions", line);
    stacks.push_stmt(new Stmt(line) );
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

    Asgn* asgn = new Asgn(acs, expr, line);  // will do type checking
    if (stmt == nullptr)
      stmt = asgn;
    else
      stmt = new Seq(asgn, stmt, line);  // will do type checking
  }

  if (stmt == nullptr)
    stmt = new Stmt(line);
  stacks.push_stmt(stmt);
}

void Actions::if_stmt(int num_cond, int line) {
  Stmt* cond = stacks.pop_stmt();
  for (int i = 0; i < num_cond - 1; i++)
    cond = new Seq(stacks.pop_stmt(), cond, line);
  stacks.push_stmt( new IfStmt(cond , line) );
}

void Actions::loop(int line) {
  stacks.push_stmt( new Loop(stacks.pop_stmt(), line) ); 
}

void Actions::empty(int line) {
  stacks.push_stmt( new Stmt(line) );
}

void Actions::condition(int num_stmts, int line) {
  Expr* cond = stacks.pop_expr();
  stmt_part(num_stmts, line);
  Stmt* stmt = stacks.pop_stmt();
  stacks.push_stmt( new Cond(cond, stmt, line) );
}

// Expression methods /////////////////////////////////////////////////

void Actions::access(int line, tag::Tag type) {
  Token* name = stacks.pop_token();
  string lexeme = name->to_string();
  Id* id = table.get(lexeme);

  if (id == nullptr) {
    error("'" + lexeme + "' is undeclared", line);
    stacks.push_stmt( new Stmt(line) );
    return;
  }

  Access* acs = nullptr;
  if (type == tag::ARRAY) {
    Expr* idx = stacks.pop_expr();
    acs = new ArrayAccess(id, idx, line);  // Can do type check on index
  } else {
    acs = new Access(id, line);
  }

  stacks.push_expr(acs);
  delete name;
}

void Actions::binary(int line) {
  Token* op = stacks.pop_token();
  Expr* lhs = stacks.pop_expr();
  Expr* rhs = stacks.pop_expr();

  stacks.push_expr( new Binary(op, lhs, rhs, line) ); 
}

void Actions::unary(tag::Tag t, int line) {
  Token* op = new Token(t);
  Expr* expr = stacks.pop_expr();

  stacks.push_expr( new Unary(op, expr, line) );
}

void Actions::constant(tag::Tag tag, int line, int val, int dec) {
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

  stacks.push_expr( new Constant(tok, type, line) );
}


// Display methods ////////////////////////////////////////////////////

void Actions::print_table() {
  cout << endl;
  cout << "=== BlockTable Nodes ===" << endl;
  table.print();
}

