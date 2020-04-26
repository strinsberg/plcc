//-----------------------------------------------------------
// interp.cc
//
// Description: PL interpreter
//
//-------------------------------------------------------------

// INCLUDES

#include <iostream>
#include <fstream>
#include <iomanip>
#include "Interpreter.h"
// FUNCTIONS

Interpreter::Interpreter( string f, bool s) : filename(f), stepping(s), op_type(symbol::OP_INT) {}

void Interpreter::interpret()
{
  //cout << " Loading..." << endl;
  load_program(filename);
  //cout << " Running ..." << endl;
  run_program();
}

void Interpreter::runtime_error( string  message, int line_number)
{
  if ( line_number != -1)
     cerr << "line " << line_number << " : " << message << endl;
  else
     cerr << message << endl;
  running = false;
}

void Interpreter::allocate( int words )
{
  for (int i = stack_register + 1; i <= stack_register + words; i++)
    store[i] = 0;

  stack_register += words;
  if (stack_register > STORE_SIZE)
     runtime_error(" stack overflow");
}

//---------------------------------
// variable_access = "variable" [ expression "index" ].
//---------------------------------

void Interpreter::variable( int level, int displacement)
{
  int x;

  // Make space for the variable address
  // and find the bottom of this block level
  allocate(1);
  x = base_register;
  
  // Move down to the address that starts the block level we need
  while ( level > 0)
  {
    x = store[x];
    --level;
  }

  // Store the address of the variable on top of the stack
  // displacement is the distance from the base of the block level
  store[stack_register] = x + displacement;
  program_register += 3;
}

void Interpreter::index( int bound, int line_number)
{
  int i = store[stack_register];
  --stack_register;

  if (i < 0 || i >= bound) {
     runtime_error("array index out of bounds: i=" + to_string(i)
                   + " bound=" + to_string(bound), -1);
  } else {
     if (op_type == symbol::OP_FLOAT)
        i *= 2;

     store[stack_register] += i;

     if (op_type == symbol::OP_FLOAT)
       store[stack_register + 1] = store[stack_register] + 1;
  }
  program_register += 3;
}

//----------------------------------------
// factor = "constant" | variable_access "value" |
//          expression | factor "not".
//-----------------------------------------

void Interpreter::constant( int value )
{
  // Make space for the value and save it to the top of the stack
  allocate(1);
  store[stack_register] = value;
  program_register += 3;
}

void Interpreter::db_constant( int value, int exp) {
  allocate(2);
  store[stack_register - 1] = value;
  store[stack_register] = exp;
  program_register += 4;
}

//-------------------------------

void Interpreter::value( symbol::OpCode type )
{
  // Replace address at top of stack with a value
  // ie. load a variable value?
  int x = store[stack_register];
  store[stack_register] = store[x];

  if (type == symbol::OP_FLOAT) {
    allocate(1);
    store[stack_register] = store[x + 1];
  }
  op_type = type;
  ++program_register;
}

void Interpreter::plnot()
{
  // Flip the bool bit. 1-0=1 and 1-1=0.
  store[stack_register] = 1 - store[stack_register];
  ++program_register;
}

//---------------------------------------
// term = factor { factor multiplying_operator}.
// multiplying_operator =
//  "multiply" | "divide" | "modulo".
//---------------------------------------

void Interpreter::multiply()
{
  ++program_register;  // Move to the next instruction

  if (op_type == symbol::OP_FLOAT) {
    stack_register -= 2;
    long long scale = store[stack_register]; 
    long long a = store[stack_register - 1];
    long long b = store[stack_register + 1];
    store[stack_register - 1] = (a * b) / scale;

  } else {
    --stack_register;  // move stack down because we are combining top 2 values

    // Take the top 2 elements on the stack and multiply them
    store[stack_register] = store[stack_register] * store[stack_register +1];
  }
}

void Interpreter::divide()
{
  ++program_register;

  if (op_type == symbol::OP_FLOAT) {
    stack_register -= 2;
    // assumes a fixed point representation with a constant scaling factor
    // scale factor is kept in the top float position instead of hard coded
    // so that it can change if desired.
    int scale = store[stack_register];
    store[stack_register - 1] =
        (store[stack_register - 1] * scale) / store[stack_register + 1];

  } else {
    --stack_register;
    store[stack_register] = store[stack_register] / store[stack_register +1];
  }
}
 
void Interpreter::modulo()
{
  ++program_register;
  --stack_register;
  store[stack_register] = store[stack_register] % store[stack_register +1];
}

//----------------------------------------------
// simple_expression = term [ "minus" ] { term adding_operator }.
//-------------------------------------------------

void Interpreter::minus()
{
   store[stack_register] *= -1;  // Negate the value at stack register
   ++program_register;
}

//---------------------------------------

void Interpreter::add()
{
  ++program_register;
  if (op_type == symbol::OP_FLOAT) {
    stack_register -= 2;
    store[stack_register - 1] = store[stack_register - 1] + store[stack_register + 1];

  } else {
    --stack_register;
    store[stack_register] = store[stack_register] + store[stack_register+1]; 
  }
}

void Interpreter::subtract()
{
  ++program_register;
  if (op_type == symbol::OP_FLOAT) {
    stack_register -= 2;
    store[stack_register - 1] = store[stack_register - 1] - store[stack_register + 1];
  } else {
    --stack_register;
    store[stack_register] = store[stack_register] - store[stack_register+1];
  }
}

//--------------------------------------------
// primary_expression = simple_expression
// [ simple_expression relational_operator ]
// relational_operator = "less" | "equal" | "greater".
//--------------------------------------------

void Interpreter::less()
{
  ++program_register;
  if (op_type == symbol::OP_FLOAT) {
    stack_register -= 3;
    store[stack_register] = (store[stack_register] < store[stack_register + 2]);
  } else {
    --stack_register;
    store[stack_register] = ( store[stack_register] < 
                          store[stack_register + 1 ]);
  }
  op_type = symbol::OP_BOOL;
}

void Interpreter::equal()
{
  ++program_register;
  if (op_type == symbol::OP_FLOAT) {
    stack_register -= 3;
    store[stack_register] = (store[stack_register] == store[stack_register + 2]);

  } else {
    --stack_register;
    store[stack_register] = ( store[stack_register] == 
                              store[stack_register + 1 ]);
  }
  op_type = symbol::OP_BOOL;
}

void Interpreter::greater()
{
  ++program_register;
  if (op_type == symbol::OP_FLOAT) {
    stack_register -= 3;
    store[stack_register] = (store[stack_register] > store[stack_register + 2]);

  } else {
    --stack_register;
    store[stack_register] = ( store[stack_register] > 
                              store[stack_register + 1 ]);
  }
  op_type = symbol::OP_BOOL;
}

void Interpreter::neq()
{
  ++program_register;
  if (op_type == symbol::OP_FLOAT) {
    stack_register -= 3;
    store[stack_register] = (store[stack_register] != store[stack_register + 2]);

  } else {
    --stack_register;
    store[stack_register] = ( store[stack_register] != 
                              store[stack_register + 1 ]);
  }
  op_type = symbol::OP_BOOL;
}

void Interpreter::leq()
{
  ++program_register;
  if (op_type == symbol::OP_FLOAT) {
    stack_register -= 3;
    store[stack_register] = (store[stack_register] <= store[stack_register + 2]);

  } else {
    --stack_register;
    store[stack_register] = ( store[stack_register] <= 
                              store[stack_register + 1 ]);
  }
  op_type = symbol::OP_BOOL;
}

void Interpreter::geq()
{
  ++program_register;
  if (op_type == symbol::OP_FLOAT) {
    stack_register -= 3;
    store[stack_register] = (store[stack_register] >= store[stack_register + 2]);

  } else {
    --stack_register;
    store[stack_register] = ( store[stack_register] >= 
                              store[stack_register + 1 ]);
  }
  op_type = symbol::OP_BOOL;
}

//--------------------------------------------
// expression =  primary_expression
// { primary_expression primary_operator }.
// primary_operator = "and" | "or".
//--------------------------------------------

void Interpreter::pland()
{
  ++program_register;
  --stack_register;
  if (store[stack_register] == 1)
    store[stack_register] = store[stack_register + 1];
}


void Interpreter::plor()
{
  ++program_register;
  --stack_register;
  if (store[stack_register] == 0)
    store[stack_register] = store[stack_register + 1];
}

//------------------------------------------------
// read_statement = variable_list "read".
// variable_list = variable_access { variable_access }.
//------------------------------------------------

void Interpreter::read(int count)
{
  int x;
//  cout << "program_register = " << program_register << endl;
//  cout << "stack_register = " << stack_register << endl;
//  cout << "base_register = " << base_register << endl;

  program_register += 3;

  // Move stack to start of variables to read into
  stack_register -= count;
  x = stack_register;
//  cout << "Count = " << count;

  while ( x < stack_register + count)
  {
    ++x;
    // could read into strings to force getting next token and making sure
    // it is the right type for the variable. Because right now like c++
    // if the token is not the right datatype it just moves on. might be ok
    // but since we are interpreting it we may as well check the type and
    // respond properly.
    if (op_type == symbol::OP_FLOAT) {
      int scale = 10000;  // Should be a constant somewhere
      double y;
      cin >> y;
      int result = int(y * scale);
      store[ store[x] ] = result;
      store[ store[x] + 1] = scale;

    } else if (op_type == symbol::OP_CHAR) {
      char y;
      cin >> y;
      store[ store[x] ] = y;

    } else if (op_type == symbol::OP_BOOL) {
      int y;
      cin >> y;
      store[ store[x] ] = (y != 0);

    } else {
      cin >> ( store[ store[x] ] );
    }
  }
}

void Interpreter::readline(int size) {
  program_register += 2;

  int address = store[stack_register];
  stack_register--;

  string line;
  getline(cin, line);

  int i = 0;
  for (; i < size - 1 and i < (int)line.size(); i++) {
    store[address + i] = line.at(i);
  }
  store[address + i] = '\0';
}


//-------------------------------------------------
// write_statement = expression_list "write".
// expression = expression { expression }.
//-------------------------------------------------

void Interpreter::write (int count, symbol::OpCode size_type)
{
  int x;
  program_register += 3;
  if (op_type == symbol::OP_FLOAT)
    count *= 2;

  stack_register -= count;
  x = stack_register;

  string sep;
  if (size_type == symbol::OP_ARRAY and op_type != symbol::OP_CHAR) {
    sep = " ";
    cout << "[";
  }

  while (x < stack_register + count )
  {
    if (op_type == symbol::OP_FLOAT) {
      int sig = store[++x];
      int scale = store[++x];
      double value = (double)sig / scale;
      cout << setprecision(11) << value;

    } else {
      int value = store[++x];
      if (op_type == symbol::OP_BOOL) {
        cout << (value == 1 ? "true" : "false");
      } else if (op_type == symbol::OP_CHAR) {
        if (value == 0) break;
        cout << (char)value;
      } else {
        cout << value;
      }
    }
    if (x < stack_register + count)
      cout << sep;
  }
  if (size_type == symbol::OP_ARRAY and op_type != symbol::OP_CHAR) {
    cout << "]";
  }
}

//---------------------------------------
// assignment_statement = variable_list expression_list.
//--------------------------------------

void Interpreter::assign (int count)
{
  int x;
  
  // Stack has count values on top of count addresses
  program_register += 2;
  stack_register = stack_register - 2*count;
  if (op_type == symbol::OP_FLOAT)
    stack_register -= count;
  x = stack_register;

  // Take the value in the expression and put it in the variable location
  while ( x < stack_register + count)
  {
    ++x;
    int address = store[x];
    store[address] = store[x + count];

    if (op_type == symbol::OP_FLOAT) {
      ++x;
      store[address + 1] = store[x + count];
    }
  }
}

//------------------------------------------
// procedure_statement = "call"
//------------------------------------------

void Interpreter::call( int level, int address)
{
  int x;

  allocate(3);
  x = base_register;

  // set x to the address of the block level the proc address is in  
  while ( level > 0)
  {
    x = store[x];
    --level;
  }

  store[stack_register - 2] = x;  // save the address of the proc block level
  store[stack_register - 1] = base_register;  // Save current block level address
  store[stack_register] = program_register + 3;  // Save the next instr address
  
  base_register = stack_register - 2;  // Set the base the proc block level
  program_register = address;  // Go to the address of proc
}

//-----------------------------------------
// if_statement = guarded_command_list "fi".
// do_statement = guarded_command_list
// guarded_command_list = guarded_command { guarded_command}.
// guarded_command = expression "arrow" statement_part "bar".
//----------------------------------------

void Interpreter::arrow( int address)
{
  if ( store[stack_register] == 1 )  // If true execute stmts
     program_register += 2;
  else
     program_register = address;  // Otherwise jump to next section
  --stack_register;
}

//----------------------------------------

void Interpreter::bar( int address)
{
  program_register = address;  // Jump past end of guarded command list
}

//-----------------------------------------

void Interpreter::fi( int line_number)
{
  // this was supposed to force if statments to cover
  // all conditions, but I do no like that as a language
  // feature and my codegen does not use FI so this should
  // never be run.
  runtime_error(" if statement fails", line_number);
}

//------------------------------------------------
// procedure_definition = "proc" block "endproc"
// block = { procedure_definition } statement_part.
// statement_part = { statement }.
// statement = empty | read_statement | write_statement
//             | assignment_statement | procedure_statement |
//             | if_statement | do_statement.
//-----------------------------------------------

void Interpreter::block(int variable_length, int address) {
  allocate(3);

  // Some of this is probably unnecessary except to provide the
  // padding expected by variable lookups in a block
  store[stack_register - 2] = base_register;  // Save the previous block address
  store[stack_register - 1] = 0;  // next two are padding for var addresses
  store[stack_register] = 0;
  
  base_register = stack_register - 2;  // Set the base the proc block level

  allocate(variable_length);  // Make space for the block variables
  program_register = address;  // Go to the block instructions address
}

void Interpreter::endblock() {
  stack_register = base_register - 1;  // top of old stack is just below this blocks base
  program_register++;  // for blocks the next instruction is just past block end
  base_register = store[base_register];  // The base of the previous block
}

void Interpreter::proc( int variable_length, int address)
{
  allocate( variable_length );  // Allocate the number of variables
  program_register = address;  // go to proc address ??
}

void Interpreter::endproc()
{
  // Reset registers after proc execution
  // See Interpreter::call
  stack_register = base_register - 1;
  program_register = store[base_register + 2];
  base_register = store[base_register + 1];
}

//--------------------------------
// program = "program" block "endprog".
//-------------------------------

void Interpreter::prog( int variable_length, int address)
{
  base_register = stack_bottom;  // address at (or after?) end of program
//  cout << "base_register = " << stack_bottom << endl;

  stack_register = base_register;
//  cout << "stack_register = " << stack_register << endl;

  allocate( variable_length + 2);  // allocate space for variables

  program_register = address;  // Set program register to start of prog instructions
//  cout << "program_register = " << program_register << endl;
}

//-----------------------------------------

void Interpreter::endprog()
{
  running = false;
}

//-------------------------------------------
// Basic operations
//-------------------------------------------

void Interpreter::load_program( string name)
{
  int x = 0;
  int word;
  const char* tempname = name.c_str(); 
  ifstream program(tempname, ios::in);
  
  if (!program)
  {
    cerr << " Could not open program file" << endl;
    return;
  }

// read program into memory

  while (!program.eof())
  {
    program >> word;
//    cout << " Read word " << word << "  Temp name: " << tempname << endl;
    store[x] = word;
    if ( word == -1) break;
    ++x;
//    cout << "x is " << x << endl;
    
    if ( x >= STORE_SIZE)
    {
      cerr << "program exceeded" << x << " words " << endl;
      cerr << "Not enough memory to load program " << endl;
      return;
     }
   }
  
   stack_bottom = x;

 //  cout << "stack_bottom = " << x << endl;
   
   // clear the rest of memory

   for (int i = stack_bottom; i <= STORE_SIZE; i++)
      store[i] = -1;
}

void Interpreter::run_program()
{
  symbol::OpCode  opcode;
  program_register = 0; 
  running = true;
  
  while (running)
  {
//
    opcode = (symbol::OpCode) store[program_register];
//
 //   cout << "Opcode = " << opcode << endl;
    if ( stepping )
    {
      cout << endl << " press < enter > to execute "
                   << symbol::op_name[opcode] << " operation" << endl;
      cin.get();
    }

    // Execute program instruction
    switch (opcode)
    {
      case symbol::OP_ADD:
         add();
         break;
      case symbol::OP_AND:
         pland();
         break;
      case symbol::OP_ARROW:
         arrow(store[program_register + 1]);
         break;
      case symbol::OP_ASSIGN:
         assign( store[program_register + 1]);
         break;
      case symbol::OP_BAR:
         bar(store[program_register + 1]);
         break;
      case symbol::OP_BLOCK:
         block(store[program_register + 1], store[program_register + 2]);
         break;
      case symbol::OP_CALL:
         call(store[program_register + 1], store[program_register + 2]);
         break;
      case symbol::OP_CONSTANT:
         op_type = (symbol::OpCode) store[program_register + 2];
         constant(store[program_register + 1]);
         break;
      case symbol::OP_DB_CONSTANT:
         op_type = (symbol::OpCode) store[program_register + 3];
         db_constant(store[program_register + 1], store[program_register + 2]);
         break;
      case symbol::OP_DIVIDE:
         divide();
         break;
      case symbol::OP_ENDBLOCK:
         endblock();
         break;
      case symbol::OP_ENDPROC:
         endproc();
         break;
      case symbol::OP_ENDPROG:
         endprog();
         break;
      case symbol::OP_EQUAL:
         equal();
         break;
      case symbol::OP_FI:
         fi( store[program_register + 1]);
         break;
      case symbol::OP_GREATER:
         greater();
         break;
      case symbol::OP_DB_INDEX:
         op_type = symbol::OP_FLOAT;
         // deliberate fallthrough to OP_INDEX
      case symbol::OP_INDEX:
         index( store[program_register + 1], store[program_register + 2]);
         break;
      case symbol::OP_LESS:
         less();
         break;
      case symbol::OP_MINUS:
         minus();
         break;
      case symbol::OP_MODULO:
         modulo();
         break;
      case symbol::OP_MULTIPLY:
         multiply();
         break;
      case symbol::OP_NOT:
         plnot();
         break;
      case symbol::OP_OR:
         plor();
         break;
      case symbol::OP_NEQ:
         neq();
         break;
      case symbol::OP_LEQ:
         leq();
         break;
      case symbol::OP_GEQ:
         geq();
         break;
      case symbol::OP_PROC:
         proc(store[program_register + 1], store[program_register + 2]);
         break;
      case symbol::OP_PROG:
         prog(store[program_register + 1], store[program_register + 2]);
         break;
      case symbol::OP_READ:
         op_type = (symbol::OpCode) store[program_register + 2];
         read(store[program_register + 1]);
         break;
      case symbol::OP_READLINE:
         readline(store[program_register + 1]);
         break;
      case symbol::OP_SUBTRACT:
         subtract();
         break;
      case symbol::OP_CHAR:
      case symbol::OP_INT:
      case symbol::OP_FLOAT:
      case symbol::OP_BOOL:
      case symbol::OP_VALUE:
         value(opcode);
         break;
      case symbol::OP_VARIABLE:
         variable(store[program_register + 1], store[program_register + 2]);
         break;
      case symbol::OP_WRITE:
         write(store[program_register + 1], (symbol::OpCode) store[program_register + 2]);
         break;
      default:
         cout << "OPCODE: " << opcode << endl;
         runtime_error(" FATAL! Damaged Program File!");
         break;
    }
  }
}

void Interpreter::memory_dump(string name) const
{
   const char *tempname = name.c_str();
   ofstream dump ( tempname, ios::out);
   
   if (!dump)
   {
     cerr << " couldn't open dump file " << endl;
     return;
   }

   int i = 0;
   
   while ( i < STORE_SIZE )
   {
     dump << store[i] << ' ';
     if ( ( i+1) % 20 == 0)
        dump << endl;
     i++;
   }
}

  
