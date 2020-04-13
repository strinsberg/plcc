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
  Expr* value = exprs.back();
  exprs.pop_back();

  var_def(SCALAR, 1, line);
  delete value;
}

void Actions::array_def(int vars, int line) {
  Expr* size = exprs.back();
  exprs.pop_back();

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
  for (int i = 0; i < vars; i++) {
    Token* name = tokens.back();
    string lexeme = name->to_string();
    Word* w = new Word(lexeme);

    if ( !table.put( lexeme, new Id(w, type, kind, line) ) ) {
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
    // need to do something to check what type is being accessed
    // for now if it is an array, there will be an access expression on
    // top of the stack that will need to be added to the expressions and
    // do some kind of array access.
    Token* name = tokens.back();
    Expr* expr = exprs.back();

    string lexeme = name->to_string();
    Id* id = table.get(lexeme);

    if (id != nullptr) {
      if (id->type == expr->type) {
        Asgn* asgn = new Asgn(id, expr, line);

        if (stmt == nullptr)
          stmt = asgn;
        else
          stmt = new Seq(asgn, stmt, line);
      } else {
        yyerror("type mismatch: " + id->to_string() + ", " + expr->to_string());
        delete expr;
      }
    } else {
      yyerror("'" + lexeme + "' is undeclared");
    }

    tokens.pop_back();
    exprs.pop_back();
    delete name;
  }

  if (stmt != nullptr)
    stmts.push_back(stmt);
}


// Expression methods /////////////////////////////////////////////////

void Actions::access(int line) {

}

void Actions::negate(int line) {

}

void Actions::constant(yytokentype type, int line) {
  Token* tok = tokens.back();
  tokens.pop_back(); 

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
