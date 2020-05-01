#include "Parser.h"
#include "Actions.h"
#include "Admin.h"
#include "AstNode.h"
#include "parser.tab.hh"
#include <cstdio>
#include <memory>
using namespace std;


// This is the only place outside of the flex and bison files that
// knows anything about them. It would be better if I could encapsulate it
// better and not mix C and C++, but this works ok.

// Pointer for the bison parser to the actions object
extern shared_ptr<Actions> actions;

// The file to use with the flex scanner
extern FILE* yyin;


Parser::Parser(shared_ptr<Admin> a, const string& f)
    : act( make_shared<Actions>(a) ), admin(a), filename(f), source(NULL) {
  actions = act;
}

Parser::~Parser() {}


shared_ptr<AstNode> Parser::parse() {
  if (set_up()) {
    yy::parser parser;
    parser.parse();
  }

  clean_up();

  return act->get_ast(); 
};


bool Parser::set_up() {
  source = fopen(filename.c_str(), "r");
  if (source == NULL) {
    admin->error("error opening input file: " + filename);
    return false;
  }
  yyin = source;
  return true;
}


void Parser::clean_up() {
  if (source != NULL)
    fclose(source);
}
