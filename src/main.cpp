#include "Compiler.h"
#include <iostream>
#include <getopt.h>
using namespace std;


int main(int argc, char** argv) {
  int opt;
  bool verbose = false, debug = false;
  bool dasm = false;    // we are running disassembler
  bool run = false;     // run the compiled file after compilation
  bool interp_only = false;  // we are running only the interpreter on the input file
  string outfile;

  // Parse command line arguments
  while((opt = getopt(argc, argv, "o:vdiDI")) != -1) {
    switch (opt) {
      case 'o':
        outfile = string(optarg);
        break;
      case 'v':
        verbose = true;
        break;
      case 'd':
        debug = true; 
        break;
      case 'D':
        dasm = true;
        break;
      case 'I':
        interp_only = true;
        break;
      case 'i':
        run = true;
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

  Compiler comp(filename, outfile, verbose, debug);
  if (dasm) {
    comp.disassemble_pl();
  } else if (interp_only) {
    comp.interpret(interp_only);
  } else {
    bool success = comp.compile_to_pl();
    if (run and success)
      comp.interpret(interp_only);
  }

  return 0;
}
