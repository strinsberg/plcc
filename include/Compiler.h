#ifndef PLCC_COMPILER_H
#define PLCC_COMPILER_H

#include "Admin.h"
#include "AstNode.h"
#include <memory>
#include <string>


class Compiler {
 public:
  Compiler() {}
  ~Compiler() {}

  void compile_to_pl();
  void dissasemble_pl();
  void interpret();

 private:
  std::string infile, outfile;
  std::shared_ptr<Admin> admin; 

  std::shared_ptr<AstNode> build_ast();
};

#endif
