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

void Actions::const_def(int line) {
  // token value is on expression stack if needed
  Token* type = *(tokens.rbegin() + 1);
  add_vars(type->tag, 1, line);

  tokens.pop_back();
  delete type;
}

void Actions::array_def(int vars, int line) {
  // size is on expression stack
  Token* type = *(tokens.rbegin() + vars);
  add_vars(type->tag, vars, line);

  tokens.pop_back();
  delete type;
}

void Actions::var_def(int vars, int line) {
  Token* type = *(tokens.rbegin() + vars);
  add_vars(type->tag, vars, line);

  tokens.pop_back();
  delete type;
}

void Actions::add_vars(yytokentype type, int vars, int line) {
  for (int i = 0; i < vars; i++) {
    Token* name = tokens.back();
    string lexeme = name->to_string();
    Word* w = new Word(lexeme);

    if ( !table.put( lexeme, new Id(w, type, line) ) ) {
      yyerror("'" + lexeme + "' already declared", false);
    }

    tokens.pop_back();
    delete name;
  }
}


// Statement methods //////////////////////////////////////////////////

void Actions::assign(int num_vars, int num_exprs, int line) {
  if (num_vars != num_exprs) {
    yyerror("number of variables does not match number of exressions", false);
    // pop everything? or just leave it?
    return;
  }

  Stmt* stmt = nullptr;
  for (int i = 0; i < num_vars; i++) {
    Token* name = tokens.back();
    tokens.pop_back();
    string lexeme = name->to_string();

    Expr* expr = exprs.back();
    exprs.pop_back();

    Id* id = table.get(lexeme);
    if (id != nullptr) {
      Asgn* asgn = new Asgn(id, expr, line);

      if (i == 0)
        stmt = asgn;
      else
        stmt = new Seq(asgn, stmt, line);

    } else {
      yyerror("'" + lexeme + "' is undeclared");
    }

    delete name;
  }

  if (stmt != nullptr)
    stmts.push_back(stmt);
}


// Expression methods /////////////////////////////////////////////////

void Actions::constant(int line) {
  Token* tok = tokens.back();
  tokens.pop_back(); 

  exprs.push_back( new Constant(tok, tok->tag, line) );
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
  cout << "=== Statment Nodes ===" << endl;
  for (auto & s : stmts) {
    cout << s->to_string() << endl;
  }

  cout << endl;
  cout << "=== Expression Nodes ===" << endl;
  for (auto & e : exprs) {
    cout << e->to_string() << endl;
  }
}

void Actions::print_table() {
  cout << endl;
  cout << "=== BlockTable Nodes ===" << endl;
  table.print();
}
