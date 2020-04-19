#include "Parser.h"
#include "Actions.h"
#include "Admin.h"
#include "AstNode.h"
#include "parser.tab.hh"
#include <memory>
using namespace std;


// Bridges the parsers need for a global Actions object to call in the rules
extern shared_ptr<Actions> actions;

Parser::Parser(shared_ptr<Admin> a) : act( make_shared<Actions>(a) ), admin(a) {
  actions = act;
}

Parser::~Parser() {}

shared_ptr<AstNode> Parser::parse() {
  yy::parser parser;
  parser.parse();
  return act->get_ast(); 
};
