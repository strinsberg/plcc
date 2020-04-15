#ifndef PLCC_PARSER_H
#define PLCC_PARSER_H

#include "AstNode.h"
#include "Actions.h"
#include "Admin.h"


class Parser {
 public:
  Parser(Admin* admin);
  ~Parser();

  AstNode* parse();

 private:
  Actions* act;
  Admin* admin;
};

#endif
