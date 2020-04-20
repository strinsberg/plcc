#include "Parser.h"
#include "Admin.h"
#include "AstNode.h"
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
        cerr << "Invalid Argument: " << (char)opt << endl;
        return EXIT_FAILURE;
    }
  }

  /* For when there is a way to read from a file
  // Check if there was an argument given for the source file to compile
  if (optind >= argc) {
    cerr << "No filename given" << endl;
    return EXIT_FAILURE;
  }

  // Open up the file to read from
  string filename(argv[optind]);
  ifstream fs(filename);

  if (!fs.is_open()) {
    cerr << "Invalid input file: " << filename << endl;
    return EXIT_FAILURE;
  }
  */

  // Maybe create a compiler class that can take a couple arguments and
  // set up and run the compilation.

  // Create necessary components
  auto admin = make_shared<Admin>(verbose);
  Parser p(admin);
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
    cout << "***Compilation failure***" << endl;
    return EXIT_FAILURE;
  }

  /* For code generation
  // Open up the file to write to
  if (outfile == "")
    outfile = "pl.out";
  ofstream ofs(outfile);

  // Open the input file to read from
  ifstream ifs("pl.asm");

  // Run the assembler 
  Assembler assembler(ifs, ofs);
  assembler.firstPass();
  ifs.seekg(0);
  assembler.secondPass();

  ifs.close();
  ofs.close();
  */

  return 0;
}
