#ifndef PLCC_PARSER_H
#define PLCC_PARSER_H

#include "AstNode.h"
#include "Actions.h"
#include "Admin.h"
#include <memory>


class Parser {
 public:
  Parser(std::shared_ptr<Admin> admin, const std::string& filename); 
  ~Parser();

  std::shared_ptr<AstNode> parse();

 private:
  std::shared_ptr<Actions> act;
  std::shared_ptr<Admin> admin;
  std::string filename;
  FILE* source;

  bool set_up();
  void clean_up();
};

#endif
