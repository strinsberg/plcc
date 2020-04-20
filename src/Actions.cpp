#include "Actions.h"
#include "Admin.h"
#include "Symbol.h"
#include "Types.h"
#include "exceptions.h"
#include <string>
#include <iostream>
#include <memory>
using namespace std;


Actions::Actions(shared_ptr<Admin> a) : admin(a), ast(empty_stmt()) {};

Actions::~Actions() {}


// Definitions ///////////////////////////////////////////////////

shared_ptr<Def> Actions::def_part(shared_ptr<Def> rest, shared_ptr<Def> last) {
  admin->debug("def part");
  if (rest->is_null())
    return last;
  return make_shared<DefSeq>(rest, last);
}


shared_ptr<Def> Actions::const_def(Type type, string name, shared_ptr<Expr> value) {
  admin->debug("const def");
  type.qual = symbol::CONST;

  auto def = make_shared<Def>();
  try {
    auto id = make_shared<ConstId>(name, type, value);
    bool added = table.put(name, id);

    if (!added)
      admin->error("'" + name + "' already declared");
    else
      def = make_shared<VarDef>(id);

  } catch (const type_error& e) {
    admin->error("type error: " + string(e.what()), name);
  }
  cout << *def << endl;
  return def;
}


shared_ptr<Def> Actions::var_def(Type type, Vars pp) {
  admin->debug("var def");

  if (pp.size == nullptr)
    type.kind = symbol::SCALAR;
  else
    type.kind = symbol::ARRAY;

  return add_vars(pp.names, type, pp.size);
}


shared_ptr<Def> Actions::proc_def(shared_ptr<Expr> id, shared_ptr<Stmt> block) {
  admin->debug("proc def");
  return make_shared<ProcDef>(id, block);
}


shared_ptr<Expr> Actions::proc_name(string name) {
  Type type = Type(symbol::UNIVERSAL, symbol::PROC, symbol::UNIVERSAL);
  auto size = make_shared<Constant>();
  auto id = make_shared<Id>(name, type, size);

  bool added = table.put(name, id);
  if (!added) {
    admin->error("'" + name + "' was not declared in this scope");
    return empty_expr();
  }

  return id;
}


// private def helpers //
shared_ptr<Def> Actions::add_vars(vector<string> names, Type type, shared_ptr<Expr> size) {
  admin->debug("add_vars");

  shared_ptr<Def> def = nullptr;
  for (auto it = names.rbegin(); it != names.rend(); it++) {
    string n = *it;
    try {
      auto id = make_shared<Id>(n, type, size);
      bool added = table.put(n, id);

      if (!added) {
        admin->error("'" + n + "' already declared");
      } else {
        auto var = make_shared<VarDef>(id);
        if (def == nullptr)
          def = var;
        else
          def = make_shared<DefSeq>(var, def);
      }
    } catch (const type_error& e) {
      admin->error("type error: " + string(e.what()), n);
    }
  }

  if (def == nullptr)
    def = make_shared<Def>();

  return def;
}


Vars Actions::vprime(vector<string> names, shared_ptr<Expr> size) {
  Vars result;
  result.size = size;
  result.names = names;
  return result;
}


// Statement methods //////////////////////////////////////////////////

shared_ptr<Stmt> Actions::block(shared_ptr<Def> defs, shared_ptr<Stmt> stmts) {
  admin->debug("block");
  table.pop_block();
  return make_shared<Block>(defs, stmts);
}


shared_ptr<Stmt> Actions::stmt_part(shared_ptr<Stmt> rest, shared_ptr<Stmt> last) {
  admin->debug("stmt part");
  if (rest->is_null())
    return last; 
  return make_shared<Seq>(rest, last);
}


shared_ptr<Stmt> Actions::io(vector<shared_ptr<Expr>> exprs, symbol::Tag type) {
  admin->debug("io");
  auto stmt = make_shared<Stmt>();
  if (exprs.size()) {
    stmt = make_shared<IoStmt>(exprs.back(), type);
    for (auto it = exprs.rbegin() + 1; it != exprs.rend(); it++) {  
      stmt = make_shared<Seq>( make_shared<IoStmt>(*it, type), stmt );
    }
  }
  return stmt;
}


shared_ptr<Stmt> Actions::assign(vector<shared_ptr<Expr>> vars, vector<shared_ptr<Expr>> values) {
  admin->debug("assign");
  if (vars.size() != values.size()) {
    admin->error("number of variables does not match number of exressions");
    return make_shared<Stmt>();
  }

  // pair each access with it's value
  shared_ptr<Stmt> stmt = nullptr;
  for (int i = vars.size() - 1; i >= 0; i--) {
    auto acs = vars.at(i);
    auto expr = values.at(i);

    auto asgn = make_shared<Stmt>();
    try {
      asgn = make_shared<Asgn>(acs, expr);
    } catch (const exception& e) {
      admin->error("type error: " + string(e.what()), acs->get_name());
    }

    if (stmt == nullptr)
      stmt = asgn;
    else
      stmt = make_shared<Seq>(asgn, stmt);
  }

  if (stmt == nullptr)
    stmt = make_shared<Stmt>();

  return stmt;
}


shared_ptr<Stmt> Actions::if_stmt(shared_ptr<Stmt> cond) {
  admin->debug("if");
  return make_shared<IfStmt>(cond);
}


shared_ptr<Stmt> Actions::loop(shared_ptr<Stmt> cond) {
  admin->debug("loop");
  return make_shared<Loop>(cond);
}


shared_ptr<Stmt> Actions::empty_stmt() {
  admin->debug("empty");
  return make_shared<Stmt>();
}


shared_ptr<Stmt> Actions::proc_stmt(string name) {
  admin->debug("call");

  auto id = get_id(name);
  if (id == nullptr)
    return make_shared<Stmt>();

  auto stmt = make_shared<Stmt>();
  try {
    stmt = make_shared<Proc>(id);
  } catch (const exception& e) {
    admin->error("type error: " + string(e.what()), name);
  }

  return stmt;
}


shared_ptr<Stmt> Actions::conditions(shared_ptr<Stmt> rest, shared_ptr<Stmt> last) {
  // like stmt part combine these into a seq
  if (rest->is_null())
    return last;
  return make_shared<Seq>(rest, last);
}


shared_ptr<Stmt> Actions::condition(shared_ptr<Expr> expr, shared_ptr<Stmt> stmts) {
  admin->debug("condition");

  auto cond = make_shared<Stmt>();
  try {
    cond = make_shared<Cond>(expr, stmts);
  } catch (const exception& e) {
    admin->error("type error: " + string(e.what()) +
                 ". actual type: " + symbol::str(expr->get_type().type));
  }

  return cond;
}


// Expression methods /////////////////////////////////////////////////

shared_ptr<Expr> Actions::access(string name, shared_ptr<Expr> idx) {
  admin->debug("access");

  auto id = get_id(name);
  if (id == nullptr)
    return make_shared<Expr>(Type());

  auto acs = make_shared<Expr>(Type());
  try {
    if (idx->get_type().type != symbol::EMPTY) {
      acs = make_shared<ArrayAccess>(id, idx);
    } else {
      acs = make_shared<Access>(id);
    }
  } catch (const exception& e) {
    admin->error("type error: " + string(e.what()), name);
  }

  return acs;
}


shared_ptr<Expr> Actions::binary(Operator op, shared_ptr<Expr> lhs, shared_ptr<Expr> rhs) {
  admin->debug("binary");

  auto bin = make_shared<Expr>(Type());
  try {
    bin = make_shared<Binary>(op, lhs, rhs);
  } catch (const type_error& e) {
    admin->error("type error: " + string(e.what()), symbol::str(op.op));
  }

  return bin; 
}


shared_ptr<Expr> Actions::unary(symbol::Tag op_type, shared_ptr<Expr> expr) {
  admin->debug("unary");
  Operator op;
  if (op_type == symbol::MINUS)
    op = new_op(symbol::MINUS, symbol::NUMBER, symbol::NUMBER);
  else
    op = new_op(symbol::NOT, symbol::BOOL, symbol::BOOL);

  auto un = make_shared<Expr>(Type());
  try {
    un = make_shared<Unary>(op, expr);
  } catch (const type_error& e) {
    admin->error("type error: " + string(e.what()), symbol::str(op.op));
  }

  return un;
}


shared_ptr<Expr> Actions::constant(symbol::Tag tag, int val, double dec) {
  admin->debug("constant: " + symbol::str(tag) + " " + to_string(val)); 
  shared_ptr<Expr> con;
  Type t;
  t.type = tag;

  if (tag == symbol::TRUE or tag == symbol::FALSE) {
    t.type = symbol::BOOL;
    t.qual = symbol::CONST;
    con = make_shared<Constant>(t, val, dec);

  } else if (tag == symbol::INT) {
    t.qual = symbol::CONST;
    con = make_shared<Constant>(t, val, dec);

  } else if (tag == symbol::FLOAT) {
    t.qual = symbol::CONST;
    int size = to_string((int)dec).size();
    for (int i = 0; i < size; i++)
      dec /= 10;
    con = make_shared<Constant>(t, 0, val + dec);

  } else if (tag == symbol::CHAR) {
    t.qual = symbol::CONST;
    con = make_shared<Constant>(t, val, dec);
  } else {
    admin->error("not a valid constant type: " + symbol::str(tag));
    con = make_shared<Expr>(Type());
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


shared_ptr<Def> Actions::new_block() {
  admin->debug("new_block");
  table.push_block();
  auto def = make_shared<Def>();
  def->set_null(true);
  return def;
}


shared_ptr<Id> Actions::get_id(string name) {
  auto id = table.get(name);
  if (id == nullptr)
    admin->error("'" + name + "' was not declared in this scope");

  return id;
}
