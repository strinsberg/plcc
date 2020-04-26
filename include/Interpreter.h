//--------------------------------------------------
// interp.h
// Description	:	Translated from Brinch Hansens PL interpreter
//-------------------------------------------------------

#ifndef PLCC_INTERPRETER_H
#define PLCC_INTERPRETER_H

// INCLUDES

#include "Symbol.h"
#include <string>
using namespace std;

// CONSTANTS

const int STORE_SIZE = 10000; // maximum size of storage


class Interpreter
{
public:

    // CONSTRUCTION

    Interpreter(string, bool = false);

    void interpret();

    // ACCESS

    void memory_dump(string) const;

private:
    void load_program( string);
    void run_program();
    void runtime_error(string, int = -1);
    void allocate( int );

    // PL MACHINE INSTRUCTION SET
    
    void variable(int, int);
    void index(int, int);

    void constant(int);
    void db_constant(int, int);
    void value(symbol::OpCode);
    void plnot();

    void multiply();
    void divide();
    void modulo();

    void minus();
    void add();
    void subtract();

    void less();
    void equal();
    void greater();

    void leq();
    void neq();
    void geq();

    void pland(); 
    void plor(); 

    void read(int);    
    void readline(int);
    void write(int, symbol::OpCode);

    void assign( int );
    
    void call( int, int);

    void arrow( int );
    void bar( int);
    void fi (int);

    void block(int, int);
    void endblock();

    void proc(int, int);
    void endproc();
  
    void prog( int, int);
    void endprog();

    // COMPONENTS
    string filename;

    int store[STORE_SIZE + 1]; // storage space
    int stack_bottom;          // bottom of the stack
    int stack_register,        // top of stack pointer (top)
        base_register,         // br
        program_register;      // pc

    bool running;              // status of the interpreter
    bool stepping;             // sets the step-by-step execution 
    symbol::OpCode op_type;    // basic data type being operated on
}; // end class Interpreter
#endif
// interp.h	
