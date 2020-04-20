#ifndef PLCC_PARSER_H
#define PLCC_PARSER_H

#include "AstNode.h"
#include "Actions.h"
#include "Admin.h"
#include <memory>


class Parser {
 public:
  Parser(std::shared_ptr<Admin> admin);
  ~Parser();

  std::shared_ptr<AstNode> parse();

 private:
  std::shared_ptr<Actions> act;
  std::shared_ptr<Admin> admin;
};

#endif
