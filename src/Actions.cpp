#include "Actions.h"
#include "Admin.h"
#include "Symbol.h"
#include "Types.h"
#include "exceptions.h"
#include <string>
#include <iostream>
#include <memory>
using namespace std;


Actions::Actions(shared_ptr<Admin> a) : admin(a), ast(make_shared<AstNode>()) {};

Actions::~Actions() {}


// Definitions ///////////////////////////////////////////////////

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
  return def;
}


shared_ptr<Def> Actions::var_def(Type type, Vars pp) {
  admin->debug("var def");

  int size = type.type == symbol::FLOAT ? 2 : 1;

  if (type.type == symbol::RECORD) {
    auto type_id = get_id(type.name);  // Will exist if we got this far?
    type.kind = symbol::RECORD;
    pp.size = make_shared<Constant>(type_id->get_size());
  } else if (pp.size == nullptr) {
    type.kind = symbol::SCALAR;
    pp.size = make_shared<Constant>(size);
  } else {
    type.kind = symbol::ARRAY;
  }

  return add_vars(pp.names, type, pp.size);
}


shared_ptr<Def> Actions::proc_def(shared_ptr<Id> id, shared_ptr<Stmt> block) {
  admin->debug("proc def");
  return make_shared<ProcDef>(id, block);
}


shared_ptr<Id> Actions::proc_name(string name) {
  admin->debug("proc name");
  Type type = Type(symbol::UNIVERSAL, symbol::PROC, symbol::UNIVERSAL);
  auto size = make_shared<Constant>();
  auto id = make_shared<Id>(name, type, size);

  if (!table.put(name, id))
    admin->error("'" + name + "' was already declared");

  return id;
}


shared_ptr<Def> Actions::rec_def(shared_ptr<Id> id, shared_ptr<DefPart> def_part) {
  admin->debug("record def");
  
  vector<shared_ptr<Id>> fields;
  for (auto& def : def_part->get_defs())
    fields.push_back(def->get_id());

  // already checked for this in rec_name, but does not hurt to ensure
  // nothing has gone wrong since then.
  if (!table.new_type(id->get_name(), fields))
    admin->error("invalid record: " + id->get_name());

  id->get_size_expr() = Constant(def_part->get_size());
  return make_shared<RecDef>(id, def_part);
}

shared_ptr<Id> Actions::rec_name(string name) {
  admin->debug("record name " + name);
  Type type = Type(symbol::RECORD, symbol::RECORD, symbol::UNIVERSAL, name);
  auto size = make_shared<Constant>();
  auto id = make_shared<Id>(name, type, size);

  // If type exists we do not want to put it's name in the table
  // type info will be added to the table in rec_def
  bool error = false;
  if (table.has_type(name))
    error = true;
  else if (!table.put(name, id))
    error = true;

  if (error)
    admin->error("type '" + name + "' was already declared");

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
  admin->debug("v prime");
  Vars result;
  result.size = size;
  result.names = names;
  return result;
}


// Statement methods //////////////////////////////////////////////////

shared_ptr<Stmt> Actions::block_stmt(shared_ptr<Stmt> block) {
  admin->debug("block stmt");
  return make_shared<BlockStmt>(block);
}

shared_ptr<Stmt> Actions::block(shared_ptr<DefPart> defs, shared_ptr<Stmt> stmts) {
  admin->debug("block");
  table.pop_block();
  return make_shared<Block>(defs, stmts);
}


shared_ptr<Stmt> Actions::stmt_part(shared_ptr<Stmt> rest, shared_ptr<Stmt> last) {
  admin->debug("stmt part\n");
  if (rest->is_null())
    return last; 
  return make_shared<Seq>(rest, last);
}


shared_ptr<Stmt> Actions::io(vector<shared_ptr<Expr>> exprs, symbol::Tag type) {
  admin->debug("io");
  auto stmt = make_shared<Stmt>();
  if (exprs.size()) {

    try {
      stmt = make_shared<IoStmt>(exprs.back(), type);

      for (auto it = exprs.rbegin() + 1; it != exprs.rend(); it++) {  
        stmt = make_shared<Seq>( make_shared<IoStmt>(*it, type), stmt );
      }

    } catch (const type_error& e) {
      admin->error("type error: " + string(e.what()));
    }
  }
  return stmt;
}


shared_ptr<Stmt> Actions::readline(string name) {
  admin->debug("readline");
  auto id = get_id(name);
  if (id == nullptr)
    return make_shared<Stmt>();

  try {
    return make_shared<ReadLine>(id);
  } catch (const type_error& e) {
    admin->error("type error: " + string(e.what()), name);
    return make_shared<Stmt>();
  }
}


shared_ptr<Stmt> Actions::assign(vector<shared_ptr<Expr>> vars,
    vector<shared_ptr<Expr>> values) {

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
      if (expr->get_type().type == symbol::STRING)
        asgn = make_shared<StringAsgn>(acs, expr);
      else
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
  admin->debug("empty stmt");
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
  admin->debug("conditions");
  if (rest->is_null())
    return last;
  return make_shared<CondSeq>(rest, last);
}


shared_ptr<Stmt> Actions::condition(shared_ptr<Expr> expr, shared_ptr<Stmt> stmts) {
  admin->debug("cond");

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
  admin->debug("access " + name);

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


shared_ptr<Expr> Actions::rec_access(
    shared_ptr<Expr> record, string field, shared_ptr<Expr> index) {
  admin->debug("record access " + record->get_name() + "." + field);

  // need to check that field is in the records def part somehow
  // then need to make an access of field using index
  // then put these into a rec access together
  auto size = make_shared<Constant>();
  Type type = Type(symbol::INT, symbol::SCALAR, symbol::UNIVERSAL);
  auto id = make_shared<Id>(field,type,size); 
  auto second = make_shared<Access>(id);

  // Need to wrap in try for type errors
  return make_shared<RecAccess>(record, second);
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


shared_ptr<Expr> Actions::constant(symbol::Tag tag, int val, string dec) {
  admin->debug("constant: " + symbol::str(tag) + " " + to_string(val)); 

  Type t;
  t.type = tag;
  t.qual = symbol::CONST;

  int scale = 10000;  // use a 4 decimal fixed point float

  if (tag == symbol::TRUE or tag == symbol::FALSE) {
    t.type = symbol::BOOL;

  } else if (tag == symbol::FLOAT) {
    // Right now we are using a 4 decimal fixed point representation
    // But later it could change to a proper floating point
    // Or we could save them as full size doubles and convert them to the
    // fixed point in the code generation. That way other code generation
    // for architechture with the capacity could treat them differently.
    if (val > 200000) {
      admin->error("fixed point float will overflow at 2,000,000.000");
      val = 200000;
    }

    string num = to_string(val) + "." + dec + "000";
    double numf = stod(num);
    val = numf * scale; 
  }

  return make_shared<Constant>(t, val, scale);
}

std::shared_ptr<Expr> Actions::const_string(std::string str) {
  admin->debug("const string");
  return make_shared<ConstString>(str);
}

// Helpers ////////////////////////////////////////////////////////////

Type Actions::new_type(symbol::Tag type, string type_name) {
  admin->debug("type: " + symbol::str(type) + " " + type_name);
  if (type_name != "")
    auto type_id = get_id(type_name);  // just to check if the type was declared

  Type t;
  t.type = type;
  t.name = type_name;

  return t;
};


Operator Actions::new_op(symbol::Tag op, symbol::Tag type, symbol::Tag qual) {
  admin->debug("op: " + symbol::str(op));
  Type t(type, symbol::OPERATOR, qual);
  return Operator(op, t);
};


shared_ptr<DefPart> Actions::new_block() {
  admin->debug("new_block");
  table.push_block();
  return make_shared<DefPart>();
}


shared_ptr<Id> Actions::get_id(string name) {
  auto id = table.get(name);
  if (id == nullptr)
    admin->error("'" + name + "' was not declared in this scope");

  return id;
}
