#include "Compiler.h"
#include <iostream>
#include <getopt.h>
using namespace std;


int main(int argc, char** argv) {
  int opt;
  bool verbose = false, debug = false;
  bool dasm = false;
  string outfile;

  // Parse command line arguments
  while((opt = getopt(argc, argv, "o:vda")) != -1) {
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
      case 'a':
        dasm = true;
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
  if (dasm)
    comp.disassemble_pl();
  else
    comp.compile_to_pl();
    // later add option to interpret after compiling

  return 0;
}
