#ifndef PLCC_COMPILER_H
#define PLCC_COMPILER_H

#include "Admin.h"
#include "AstNode.h"
#include <memory>
#include <string>


class Compiler {
 public:
  Compiler(std::string infile, std::string outfile, bool verbose, bool debug);
  ~Compiler() {}

  void compile_to_pl();
  void disassemble_pl();
  void interpret();

 private:
  std::string infile, outfile;
  bool verbose;
  std::shared_ptr<Admin> admin; 

  std::shared_ptr<AstNode> build_ast();
  void pl_code_gen(std::shared_ptr<AstNode> ast);
};

#endif
