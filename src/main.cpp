#include "Parser.h"
#include "Admin.h"
#include "AstNode.h"
#include "CodeGenPL.h"
#include <iostream>
#include <memory>
#include <getopt.h>
#include <fstream>
using namespace std;


int main(int argc, char** argv) {
  int opt;
  bool verbose = false;

  // Deal with command line arguments
  string outfile;
  while((opt = getopt(argc, argv, "o:v")) != -1) {
    switch (opt) {
      case 'o':
        outfile = string(optarg);
        break;
      case 'v':
        verbose = true;
        break;
      default:
        cerr << "error: invalid option: " << (char)opt << endl;
        return EXIT_FAILURE;
    }
  }

  // Check if there was an argument given for the source file to compile
  if (optind >= argc) {
    cerr << "error: no filename given" << endl;
    return EXIT_FAILURE;
  }

  // Open up the file to read from
  string filename(argv[optind]);

  // Create necessary components
  auto admin = make_shared<Admin>(verbose);
  Parser p(admin, filename);
  shared_ptr<AstNode> ast = p.parse();

  // Display the tree
  if (verbose) {
    cout << endl;
    cout << "==== Syntax Tree ====" << endl;
    TreeWalker walker;
    walker.walk(*ast);
  }

  // If there are errors then quit. If MAX_ERRORS are reached the admin will
  // exit(EXIT_FAILURE) during parsing.
  if (admin->get_errors() > 0) {
    cout << endl;
    cout << "***compilation failure***" << endl;
    cout << "number of errors: " << admin->get_errors() << endl;
    return EXIT_FAILURE;
  }

  // Here we would walk the ast with the code generator
  // It will take an output file to write the code into
  if (verbose)
    cout << endl << "Generating Code... " << endl << endl;
  CodeGenPL gen(&cout);
  gen.walk(*ast);

  // After this would bw were interfacing with other systems would be
  // done to assemble the code. For PL it would be the PL_assembler.
  // In the future if LLVM is used we would either find a way to
  // call a program to compile the LLVM to machine code or just
  // skip the step and let the user compile on their own. Maybe
  // even write a python script for this to interface more easily
  // with other external programs after the assembly is done.
  // This could even be set up to allow compilation and interpretation
  // for PL in one step.

  return 0;
}
