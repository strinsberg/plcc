#include "Parser.h"
#include "Actions.h"
#include "Admin.h"
#include "AstNode.h"
#include "parser.tab.hh"

// Bridges the parsers need for a global Actions object to call in the rules
extern Actions* actions;

Parser::Parser(Admin* a) : act( new Actions(a) ), admin(a) {
  actions = act;
}

Parser::~Parser() {
  delete act;
}

AstNode* Parser::parse() {
  yy::parser parser;
  parser.parse();
  return act->get_ast(); 
};
