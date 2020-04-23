#ifndef DASM_H
#define DASM_H

#include "Admin.h"
#include <memory>
#include <fstream>
#include <iostream>


/**
 * Class to disassemble valid PL programs into a commented
 * assembly code.
 */
class Dasm {
 public:
  /**
   * Creates a new disassembler.
   * @param fs The input file stream for the program to dissasemble.
   * @param ofs The output file stream to store the assembly code.
   * @param admin A pointer to the administration object.
   * printed to stdout.
   */
  Dasm(std::ifstream & fs, std::ofstream & ofs, std::shared_ptr<Admin> admin);
  ~Dasm() {}

  /**
   * Disassemble the program and store the results in the output file.
   */
  void disassemble();

 private:
  std::ifstream* in;    // The file to read from
  std::ofstream* out;   // The file to store results
  std::shared_ptr<Admin> admin;

  /**
   * Gets the next integer from the input.
   * Main purpose is to display the integers that are read in as OP_CODES when
   * verbose mode is true.
   * If verbose is true the OP_CODE and the instruction name will be printed
   * to stdout. If the integer read in is not a valid OP_CODE "Invalid OP_CODE"
   * will be printed instead of the name.
   * The return value does not change if the OP_CODE is not valid. This is for
   * the caller to handle.
   * @return The OP_CODE or -1 if the input is finished.
   */
  int get_inst();
};

#endif
