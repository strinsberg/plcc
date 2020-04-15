#include "Admin.h"
#include "AstNode.h"
#include "parser.tab.h"

// Bridges the parsers need for a global Actions object to call in the rules
extern Actions* actions;

Admin::Admin() : act( new Actions() ) {
  actions = act;
}

Admin::~Admin() {
  delete act;
}

AstNode* Admin::parse() {
  yyparse();
  act->print_nodes();
  return act->ast(); 
};
