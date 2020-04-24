#include "Dasm.h"
#include "Admin.h"
#include <memory>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;


// A map from OP_CODES to their psuedo code equivilent
const vector<string> OPS{
  "ADD", "AND", "ARROW", "ASSIGN",
  "BAR", "CALL", "CONSTANT", "DIVIDE",
  "ENDPROC", "ENDPROG", "EQUAL",
  "FI", "GREATER", "INDEX", "LESS",
  "MINUS", "MODULO", "MULTIPLY",
  "NOT", "OR", "PROC", "PROG",
  "READ", "SUBTRACT",
  "VALUE", "VARIABLE", "WRITE", "BLOCK", "ENDBLOCK",
  "CHAR", "INT", "FLOAT", "BOOL", "DB_CONSTANT"
};


Dasm::Dasm(std::ifstream & fs, std::ofstream & ofs, shared_ptr<Admin> a)
    : in(&fs), out(&ofs), admin(a) {}


void Dasm::disassemble() {
  int temp;               // Temp variable to hold non-instruction values
  int inst = get_inst();  // The OP_CODE for the next instruction
  string op;              // The pseudo name for an instructions

  // Loop and resolve instructions until ENDPROC
  while (true) {
    // If inst is not a valid OP_CODE display error and quit
    try {
      op = OPS.at(inst);
    } catch (out_of_range) {
      admin->error("Invalid OpCode: " + to_string(inst));
      break;
    }

    // output the instruction name
    *out << op << endl;
    admin->newline();

    // Do extra for instructions that are more than just a name
    if (op == "ENDPROG") {
      admin->newline();
      break;

    // Instructions that have 2 integers following them
    } else if (op == "PROG") {
      *in >> temp;
      *out << setw(20) << left << temp << "% Number of variables in PROG block" << endl;
      admin->newline();

      *in >> temp;
      *out << setw(20) << left << temp << "% Start address of PROG instructions" << endl;
      admin->newline();

    } else if (op == "BLOCK") {
      *in >> temp;
      *out << setw(20) << left << temp << "% Number of variables in the BLOCK" << endl;
      admin->newline();

      *in >> temp;
      *out << setw(20) << left << temp << "% Start address of BLOCK instructions" << endl;
      admin->newline();


    } else if (op == "PROC") {
      *in >> temp;
      *out << setw(20) << left << temp << "% Number of variables in PROC block" << endl;
      admin->newline();

      *in >> temp;
      *out << setw(20) << left << temp << "% Start address of PROC instructions" << endl;
      admin->newline();

    } else if (op == "VARIABLE") {
      *in >> temp;
      *out << setw(20) << left << temp << "% Number of blocks the variable is above the current block" << endl;
      admin->newline();

      *in >> temp;
      *out << setw(20) << left << temp << "% Address of variable in its block" << endl;
      admin->newline();

    } else if (op == "CALL") {
      *in >> temp;
      *out << setw(20) << left << temp << "% Number of blocks the PROC definition is above the current block" << endl;
      admin->newline();

      *in >> temp;
      *out << setw(20) << left << temp << "% The start address of the procedure instructions" << endl;
      admin->newline();

    } else if (op == "INDEX") {
      *in >> temp;
      *out << setw(20) << left << temp << "% The size of the variable being accessed" << endl;
      admin->newline();

      *in >> temp;
      *out << setw(20) << left << temp << "% The line number of the access" << endl;
      admin->newline();

    } else if (op == "DB_CONSTANT") {
      *in >> temp;
      *out << setw(20) << left << temp << "% The significand" << endl;
      admin->newline();

      *in >> temp;
      *out << setw(20) << left << temp << "% The exponent" << endl;
      admin->newline();

    // Instructions that have a single integer following them
    } else if (op == "CONSTANT") {
      *in >> temp;
      *out << setw(20) << left << temp << "% The value of the constant" << endl;
      admin->newline();

    } else if (op == "ARROW") {
      *in >> temp;
      *out << setw(20) << left << temp << "% The jump address if the condition fails" << endl;
      admin->newline();

    } else if (op == "BAR") {
      *in >> temp;
      *out << setw(20) << left << temp << "% The jump address to past end of if-stmt or beginnig of loop stmts" << endl;
      admin->newline();

    } else if (op == "READ") {
      *in >> temp;
      *out << setw(20) << left << temp << "% The number of variables to read into" << endl;
      admin->newline();

    } else if (op == "WRITE") {
      *in >> temp;
      *out << setw(20) << left << temp << "% The number of expressions to write" << endl;
      admin->newline();

    } else if (op == "ASSIGN") {
      *in >> temp;
      *out << setw(20) << left << temp << "% The number of assignments being made" << endl;
      admin->newline();

    } else if (op == "FI") {
      *in >> temp;
      *out << setw(20) << left << temp << "% The line number of the FI statement" << endl;
      admin->newline();
     
    }

    // Get the next instruction
    inst = get_inst();
  }

}


int Dasm::get_inst() {
  int inst;

  // If there is still input get an instructions
  if (*in >> inst) {
    admin->newline();

    if (inst < (int)OPS.size())
      admin->debug(OPS[inst] + ": " + to_string(inst));

    // Return the OP_CODE
    return inst;
  }

  // The input has been used up
  return -1;
}

