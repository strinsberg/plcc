#include "Actions.h"
#include "Admin.h"
#include "Symbol.h"
#include "Types.h"
#include "exceptions.h"
#include <string>
#include <iostream>
#include <memory>
using namespace std;


Actions::Actions(std::shared_ptr<Admin> a) : admin(a), ast(empty_stmt()) {};

Actions::~Actions() {}


// Definitions ///////////////////////////////////////////////////

std::shared_ptr<Def> Actions::def_part(std::shared_ptr<Def> rest, std::shared_ptr<Def> last) {
  admin->debug("def part");
  if (rest == nullptr)
    return last;
  return make_shared<Def>( DefSeq(rest, last) );
}


std::shared_ptr<Def> Actions::const_def(Type type, std::string name, std::shared_ptr<Expr> value) {
  admin->debug("const def");
  type.qual = symbol::CONST;

  auto def = make_shared<Def>( Def() );
  try {
    auto id = make_shared<Id>( ConstId(name, type, value) );
    bool added = table.put(name, id);

    if (!added)
      admin->error("'" + name + "' already declared");
    else
      def = make_shared<Def>( VarDef(id) );

  } catch (const type_error& e) {
    admin->error("type error: " + string(e.what()), name);
  }
  cout << *def << endl;
  return def;
}


std::shared_ptr<Def> Actions::var_def(Type type, Vp pp) {
  admin->debug("var def");

  if (pp.size == nullptr)
    type.kind = symbol::SCALAR;
  else
    type.kind = symbol::ARRAY;

  return add_vars(pp.names, type, pp.size);
}


std::shared_ptr<Def> Actions::proc_def(std::shared_ptr<Expr> id, std::shared_ptr<Stmt> block) {
  admin->debug("proc def");
  return make_shared<Def>( ProcDef(id, block) );
}


std::shared_ptr<Expr> Actions::proc_name(std::string name) {
  Type type = Type(symbol::UNIVERSAL, symbol::PROC, symbol::UNIVERSAL);
  std::shared_ptr<Expr> size = make_shared<Expr>( Constant() );
  std::shared_ptr<Id> id = make_shared<Id>( Id(name, type, size) );

  bool added = table.put(name, id);
  if (!added) {
    admin->error("'" + name + "' was not declared in this scope");
    return empty_expr();
  }

  return id;
}


// private def helpers //
std::shared_ptr<Def> Actions::add_vars(vector<string> names, Type type, std::shared_ptr<Expr> size) {
  admin->debug("add_vars");

  std::shared_ptr<Def> def = nullptr;
  for (auto it = names.rbegin(); it != names.rend(); it++) {
    string n = *it;
    try {
      std::shared_ptr<Id> id = make_shared<Id>( Id(n, type, size) );
      bool added = table.put(n, id);

      if (!added) {
        admin->error("'" + n + "' already declared");
      } else {
        auto var = make_shared<Def>( VarDef(id) );
        if (def == nullptr)
          def = var;
        else
          def = make_shared<Def>( DefSeq(var, def) );
      }
    } catch (const type_error& e) {
      admin->error("type error: " + string(e.what()), n);
    }
  }

  if (def == nullptr)
    def = make_shared<Def>( Def() );

  return def;
}


Vp Actions::vprime(std::vector<std::string> names, std::shared_ptr<Expr> size) {
  Vp result;
  result.size = size;
  result.names = names;
  return result;
}


// Statement methods //////////////////////////////////////////////////

std::shared_ptr<Stmt> Actions::block(std::shared_ptr<Def> defs, std::shared_ptr<Stmt> stmts) {
  admin->debug("block");
  table.pop_block();
  return make_shared<Stmt>( Block(defs, stmts) );
}


std::shared_ptr<Stmt> Actions::stmt_part(std::shared_ptr<Stmt> rest, std::shared_ptr<Stmt> last) {
  admin->debug("stmt part");
  if (rest == nullptr)
    return last; 
  return make_shared<Stmt>( Seq(rest, last) );
}


std::shared_ptr<Stmt> Actions::io(std::vector<std::shared_ptr<Expr>> exprs, symbol::Tag type) {
  admin->debug("io");
  std::shared_ptr<Stmt> stmt = make_shared<Stmt>( IoStmt(exprs.back(), type) );
  for (auto it = exprs.rbegin() + 1; it != exprs.rend(); it++) {  
    stmt = make_shared<Stmt>( Seq( make_shared<Stmt>(IoStmt(*it, type)), stmt ) );
  }
  return stmt;
}


std::shared_ptr<Stmt> Actions::assign(vector<std::shared_ptr<Expr>> vars, vector<std::shared_ptr<Expr>> values) {
  admin->debug("assign");
  if (vars.size() != values.size()) {
    admin->error("number of variables does not match number of exressions");
    return make_shared<Stmt>( Stmt() );
  }

  // pair each access with it's value
  std::shared_ptr<Stmt> stmt = nullptr;
  for (int i = vars.size() - 1; i >= 0; i--) {
    auto acs = vars.at(i);
    auto expr = values.at(i);

    std::shared_ptr<Stmt> asgn = make_shared<Stmt>( Stmt() );
    try {
      asgn = make_shared<Stmt>( Asgn(acs, expr) );
    } catch (const exception& e) {
      admin->error("type error: " + string(e.what()), acs->get_name());
    }

    if (stmt == nullptr)
      stmt = asgn;
    else
      stmt = make_shared<Stmt>( Seq(asgn, stmt) );
  }

  if (stmt == nullptr)
    stmt = make_shared<Stmt>( Stmt() );

  return stmt;
}


std::shared_ptr<Stmt> Actions::if_stmt(std::shared_ptr<Stmt> cond) {
  admin->debug("if");
  return make_shared<Stmt>( IfStmt(cond) );
}


std::shared_ptr<Stmt> Actions::loop(std::shared_ptr<Stmt> cond) {
  admin->debug("loop");
  return make_shared<Stmt>( Loop(cond) );
}


std::shared_ptr<Stmt> Actions::empty_stmt() {
  admin->debug("empty");
  return make_shared<Stmt>( Stmt() );
}


std::shared_ptr<Stmt> Actions::proc_stmt(std::string name) {
  admin->debug("call");

  auto id = get_id(name);
  if (id == nullptr)
    return make_shared<Stmt>( Stmt() );

  std::shared_ptr<Stmt> stmt = make_shared<Stmt>( Stmt() );
  try {
    stmt = make_shared<Stmt>( Proc(id) );
  } catch (const exception& e) {
    admin->error("type error: " + string(e.what()), name);
  }

  return stmt;
}


std::shared_ptr<Stmt> Actions::conditions(std::shared_ptr<Stmt> rest, std::shared_ptr<Stmt> last) {
  // like stmt part combine these into a seq
  if (rest == nullptr)
    return last;
  return make_shared<Stmt>( Seq(rest, last) );
}


std::shared_ptr<Stmt> Actions::condition(std::shared_ptr<Expr> expr, std::shared_ptr<Stmt> stmts) {
  admin->debug("condition");

  std::shared_ptr<Stmt> cond = make_shared<Stmt>( Stmt() );
  try {
    cond = make_shared<Stmt>( Cond(expr, stmts) );
  } catch (const exception& e) {
    admin->error("type error: " + string(e.what()) +
                 ". actual type: " + symbol::str(expr->get_type().type));
  }

  return cond;
}


// Expression methods /////////////////////////////////////////////////

std::shared_ptr<Expr> Actions::access(string name, std::shared_ptr<Expr> idx) {
  admin->debug("access");

  auto id = get_id(name);
  if (id == nullptr)
    return make_shared<Expr>( Expr(Type()) );

  std::shared_ptr<Expr> acs = make_shared<Expr>( Expr(Type()) );
  try {
    if (idx->get_type().type != symbol::EMPTY) {
      acs = make_shared<Expr>( ArrayAccess(id, idx) );
    } else {
      acs = make_shared<Expr>( Access(id) );
    }
  } catch (const exception& e) {
    admin->error("type error: " + string(e.what()), name);
  }

  return acs;
}


std::shared_ptr<Expr> Actions::binary(Operator op, std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs) {
  admin->debug("binary");

  auto bin = make_shared<Expr>( Expr(Type()) );
  try {
    bin = make_shared<Expr>( Binary(op, lhs, rhs) );
  } catch (const type_error& e) {
    admin->error("type error: " + string(e.what()), symbol::str(op.op));
  }

  return bin; 
}


std::shared_ptr<Expr> Actions::unary(symbol::Tag op_type, std::shared_ptr<Expr> expr) {
  admin->debug("unary");
  Operator op;
  if (op_type == symbol::MINUS)
    op = new_op(symbol::MINUS, symbol::NUMBER, symbol::NUMBER);
  else
    op = new_op(symbol::NOT, symbol::BOOL, symbol::BOOL);

  auto un = make_shared<Expr>( Expr(Type()) );
  try {
    un = make_shared<Expr>( Unary(op, expr) );
  } catch (const type_error& e) {
    admin->error("type error: " + string(e.what()), symbol::str(op.op));
  }

  return un;
}


std::shared_ptr<Expr> Actions::constant(symbol::Tag tag, int val, double dec) {
  admin->debug("constant: " + symbol::str(tag) + " " + to_string(val)); 
  std::shared_ptr<Expr> con;
  Type t;
  t.type = tag;

  if (tag == symbol::TRUE or tag == symbol::FALSE) {
    t.type = symbol::BOOL;
    t.qual = symbol::CONST;
    con = make_shared<Expr>( Constant(t, val, dec) );

  } else if (tag == symbol::INT) {
    t.qual = symbol::CONST;
    con = make_shared<Expr>( Constant(t, val, dec) );

  } else if (tag == symbol::FLOAT) {
    t.qual = symbol::CONST;
    int size = to_string((int)dec).size();
    for (int i = 0; i < size; i++)
      dec /= 10;
    con = make_shared<Expr>( Constant(t, 0, val + dec) );

  } else if (tag == symbol::CHAR) {
    t.qual = symbol::CONST;
    con = make_shared<Expr>( Constant(t, val, dec) );
  } else {
    admin->error("not a valid constant type: " + symbol::str(tag));
    con = make_shared<Expr>( Expr(Type()) );
  }

  return con;
}


// Helpers ////////////////////////////////////////////////////////////

Type Actions::new_type(symbol::Tag type) {
  admin->debug("type: " + symbol::str(type));
  Type t;
  t.type = type;
  return t;
};


Operator Actions::new_op(symbol::Tag op, symbol::Tag type, symbol::Tag qual) {
  admin->debug("op: " + symbol::str(op));
  Type t(type, symbol::OPERATOR, qual);
  return Operator(op, t);
};


void Actions::new_block() {
  admin->debug("new_block");
  table.push_block();
}


std::shared_ptr<Id> Actions::get_id(string name) {
  auto id = table.get(name);
  if (id == nullptr)
    admin->error("'" + name + "' was not declared in this scope");

  return id;
}
