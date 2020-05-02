#include "Compiler.h"
#include "Admin.h"
#include "AstNode.h"
#include "Parser.h"
#include "Dasm.h"
#include "CodeGenPL.h"
#include "Interpreter.h"
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
using namespace std;


Compiler::Compiler(string in, string out, bool v, bool debug)
  : infile(in), outfile(out), verbose(v), admin( make_shared<Admin>(debug) ) {}

bool Compiler::compile_to_pl() {
  shared_ptr<AstNode> ast = build_ast();

  /* Disable this for now while working on language and AST
  if (ast != nullptr)
    pl_code_gen(ast);
  */

  return admin->get_errors() == 0;
}

void Compiler::disassemble_pl() {
  ifstream ifs(infile);

  if (outfile == "") {
    outfile = "pl.dasm";                                                          
  }
  ofstream ofs(outfile);

  Dasm d(ifs, ofs, admin);
  d.disassemble();
}

void Compiler::interpret(bool solo) {
  // Should add the admin class to this so that it can take care of
  // debuging and errors.
  string file = solo ? infile : outfile;
  Interpreter interp(file, false);
  interp.interpret();
}


shared_ptr<AstNode> Compiler::build_ast() {
  Parser p(admin, infile);
  admin->debug("Parsing...\n");
  shared_ptr<AstNode> ast = p.parse();

  // Display the tree
  if (verbose) {
    cout << endl;
    cout << "==== Syntax Tree ====" << endl;
    TreeWalker walker;
    walker.walk(*ast);
  }

  // If there are errors return nullptr
  if (admin->get_errors() > 0) {
    cerr<< "***compilation failure***" << endl;
    cerr << "number of errors: " << admin->get_errors() << endl;
    return nullptr;
  }

  return ast;
}

void Compiler::pl_code_gen(shared_ptr<AstNode> ast) {
  admin->debug("\nGenerating Code...\n");

  if (outfile == "")
    outfile = "pl.asm";
  ofstream ofs(outfile);

  CodeGenPL gen(admin, &ofs);
  gen.walk(*ast);

  ofs.close();
}
