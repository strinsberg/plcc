#include "Actions.h"
#include <string>
#include <iostream>

using namespace std;

extern void yyerror(string, bool is_near = true);


Actions::~Actions() {
  for (auto & t : tokens)
    delete t;

  for (auto & e : exprs)
    delete e;

  for (auto & s : stmts)
    delete s;
}


// Token addition /////////////////////////////////////////////////////
void Actions::add_t(yytokentype t) {
  tokens.push_back(new Token(t));
}

void Actions::add_w(std::string s) {
  tokens.push_back(new Word(s));
}

void Actions::add_n(int n) {
  tokens.push_back(new Number(n));
}

void Actions::add_f(int n, int d) {
  tokens.push_back(new Float(n, d));
}

void Actions::add_c(char c) {
  tokens.push_back(new Char(c));
}


// Definitions ///////////////////////////////////////////////////

void Actions::def_part(int num_defs, int line) {
  Def* def = next_def();
  for (int i = 0; i < num_defs - 1; i++ ) {
    def = new DefSeq( next_def(), def, line);
  }
  defs.push_back(def);
}

void Actions::const_def(int line) {
  Expr* value = next_expr();
  var_def(SCALAR, 1, line);
  delete value;
}

void Actions::array_def(int vars, int line) {
  Expr* size = next_expr();
  var_def(ARRAY, vars, line);
  delete size;
}

void Actions::var_def(yytokentype kind, int vars, int line) {
  Token* type = *(tokens.rbegin() + vars);
  add_vars(type->tag, kind, vars, line);
  tokens.pop_back();
  delete type;
}

void Actions::add_vars(yytokentype type, yytokentype kind, int vars, int line) {
  Def* def = nullptr;
  for (int i = 0; i < vars; i++) {
    Token* name = next_token();
    string lexeme = name->to_string();
    Word* w = new Word(lexeme);

    Id* id = new Id(w, type, kind, line);
    bool added = table.put(lexeme, id);

    if (!added) {
      yyerror("'" + lexeme + "' already declared", false);
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

  defs.push_back(def);
}


// Statement methods //////////////////////////////////////////////////

void Actions::block(int num_defs, int num_stmts, int line) {
  def_part(num_defs, line);
  stmt_part(num_stmts, line);
  stmts.push_back( new Block(next_def(), next_stmt(), line) );
  table.pop_block();
}

void Actions::stmt_part(int num_stmts, int line) {
  Stmt* stmt = next_stmt();
  for (int i = 0; i < num_stmts - 1; i++) {
    stmt = new Seq( next_stmt(), stmt, line ); 
  }
  stmts.push_back(stmt);
}

void Actions::write(int num_expr, int line) {
  Stmt* stmt = new Write(next_expr(), line);
  for (int i = 0; i < num_expr - 1; i++) {  
    stmt = new Seq( new Write(next_expr(), line), stmt, line );
  }
  stmts.push_back(stmt);
}

void Actions::assign(int num_vars, int num_exprs, int line) {
  if (num_vars != num_exprs) {
    yyerror("number of variables does not match number of exressions", false);
    // pop everything? or just leave it?
    return;
  }

  // get all the expressions
  vector<Expr*> rhs(num_vars);
  for (auto & e : rhs) e = next_expr();

  vector<Expr*> lhs(num_vars);
  for (auto & e : lhs) e = next_expr();

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

  if (stmt != nullptr) {
    stmts.push_back(stmt);
  } else {
    yyerror("nothing was assigned");
  }
}

void Actions::if_stmt(int num_cond, int line) {
  Stmt* cond = next_stmt();
  for (int i = 0; i < num_cond - 1; i++)
    cond = new Seq(next_stmt(), cond, line);
  stmts.push_back( new IfStmt(cond , line) );
}

void Actions::loop(int line) {
  stmts.push_back( new Loop(next_stmt(), line) ); 
}

void Actions::empty(int line) {
  stmts.push_back( new Stmt(line) );
}

void Actions::condition(int num_stmts, int line) {
  Expr* cond = next_expr();
  stmt_part(num_stmts, line);
  Stmt* stmt = next_stmt();
  stmts.push_back( new Cond(cond, stmt, line) );
}

// Expression methods /////////////////////////////////////////////////

void Actions::access(int line, yytokentype type) {
  Token* name = next_token();
  string lexeme = name->to_string();
  Id* id = table.get(lexeme);

  if (id == nullptr) {
    yyerror("'" + lexeme + "' is undeclared");
    return;
  }

  Access* acs = nullptr;
  if (type == ARRAY) {
    Expr* idx = next_expr();
    acs = new ArrayAccess(id, idx, line);  // Can do type check on index
  } else {
    acs = new Access(id, line);
  }

  exprs.push_back(acs);
  delete name;
}

void Actions::binary(int line) {
  Token* op = next_token();
  Expr* lhs = next_expr();
  Expr* rhs = next_expr();

  exprs.push_back( new Binary(op, lhs, rhs, line) ); 
}

void Actions::unary(yytokentype t, int line) {
  Token* op = new Token(t);
  Expr* expr = next_expr();

  exprs.push_back( new Unary(op, expr, line) );
}

void Actions::constant(yytokentype type, int line) {
  Token* tok = next_token();
  exprs.push_back( new Constant(tok, type, line) );
}


// Display methods ////////////////////////////////////////////////////
void Actions::print_tokens() {
  cout << endl;
  cout << "=== Token Stack ===" << endl;
  for (auto & t : tokens) {
    cout << t->to_string() << endl;
  }
}

void Actions::print_nodes() {
  cout << endl;
  cout << "=== Definition Nodes: " << defs.size() << " ===" << endl;
  for (auto & d : defs) {
    cout << d->to_string() << endl;
  }

  cout << endl;
  cout << "=== Statment Nodes: "<< stmts.size() << " ===" << endl;
  for (auto & s : stmts) {
    cout << s->to_string() << endl;
  }

  cout << endl;
  cout << "=== Expression Nodes: " << exprs.size() << " ===" << endl;
  for (auto & e : exprs) {
    cout << e->to_string() << endl;
  }
}

void Actions::print_table() {
  cout << endl;
  cout << "=== BlockTable Nodes ===" << endl;
  table.print();
}


// Helpers ////////////////////////////////////////////////////////////
// These could return empty objects if the stack is empty
Token* Actions::next_token() {
  Token* next = tokens.back();
  tokens.pop_back();
  return next;
}

Expr* Actions::next_expr() {
  Expr* next = exprs.back();
  exprs.pop_back();
  return next;
}

Stmt* Actions::next_stmt() {
  Stmt* next = stmts.back();
  stmts.pop_back();
  return next;
}

Def* Actions::next_def() {
  Def* next = defs.back();
  defs.pop_back();
  return next;
}
