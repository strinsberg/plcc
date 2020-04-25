#ifndef PLCC_SYMBOL_H
#define PLCC_SYMBOL_H

#include <string>
#include <map>

namespace symbol {

enum Tag {
  BEG = 256, END,
  COMMA, DOT, SEMI,
  LHRND, RHRND, LHSQR, RHSQR,
  WRITE, ASGN, IF, DO, ELIF, ENDIF, LOOP, ENDLOOP, SKIP,
  AND, OR, NOT,
  INIT, EQ, NEQ, LESS, GREATER, LEQ, GEQ,
  PLUS, MINUS, MULT, DIV, MOD,
  ARRAY, PROC, ENDPROC, RECORD, ENDREC, TYPE, SCALAR,
  INT, BOOL, FLOAT, CHAR, CONST, STRING,
  NUMBER, TRUE, FALSE, NAME, CHARACTER,
  EMPTY, CALL, READ, UNIVERSAL, OPERATOR,
};


static const std::map<Tag, std::string> to_string {
  {BEG, "BEGIN"},
  {END, "END"},
  {COMMA, ","},
  {DOT, "."},
  {SEMI, ";"},
  {LHRND, "("},
  {RHRND, ")"},
  {LHSQR, "["},
  {RHSQR, "["},
  {WRITE, "WRITE"},
  {ASGN, ":="},
  {IF, "IF"},
  {DO, "DO"},
  {ELIF, "ELIF"},
  {ENDIF, "ENDIF"},
  {LOOP, "LOOP"},
  {ENDLOOP, "ENDLOOP"},
  {SKIP, "SKIP"},
  {AND, "AND"},
  {OR, "OR"},
  {NOT, "NOT"},
  {INIT, "="},
  {EQ, "=="},
  {NEQ, "~="},
  {LESS, "<"},
  {GREATER, ">"},
  {LEQ, "<="},
  {GEQ, ">="},
  {PLUS, "+"},
  {MINUS, "-"},
  {MULT, "*"},
  {DIV, "/"},
  {MOD, "%"},
  {SCALAR, "SCALAR"},
  {ARRAY, "ARRAY"},
  {PROC, "PROC"},
  {ENDPROC, "ENDPROC"},
  {RECORD, "RECORD"},
  {ENDREC, "ENDREC"},
  {TYPE, "TYPE"},
  {INT, "INT"},
  {BOOL, "BOOL"},
  {FLOAT, "FLOAT"},
  {CHAR, "CHAR"},
  {STRING, "STRING"},
  {CONST, "CONST"},
  {NUMBER, "NUMBER"},
  {TRUE, "TRUE"},
  {FALSE, "FALSE"},
  {NAME, "NAME"}, 
  {CHARACTER, "CHARACTER"},
  {EMPTY, "EMPTY"},
  {CALL, "CALL"},
  {READ, "READ"},
  {UNIVERSAL, "UNIVERSAL"},
  {OPERATOR, "OPERATOR"},
};


// To simplify to string calls. Not sure inline is good practice, but
// something is necessary to define it here without a source file.
inline std::string str(Tag t) {
  return to_string.at(t);
}

// Opcodes and their values for the interpreter
enum OpCode {
  OP_ADD, OP_AND, OP_ARROW, OP_ASSIGN, OP_BAR, OP_CALL, OP_CONSTANT,
  OP_DIVIDE, OP_ENDPROC, OP_ENDPROG, OP_EQUAL, OP_FI, OP_GREATER,
  OP_INDEX, OP_LESS, OP_MINUS, OP_MODULO, OP_MULTIPLY, OP_NOT,
  OP_OR, OP_PROC, OP_PROG, OP_READ, OP_SUBTRACT, OP_VALUE, OP_VARIABLE,
  OP_WRITE, OP_BLOCK, OP_ENDBLOCK, OP_CHAR, OP_INT, OP_FLOAT, OP_BOOL,
  OP_DB_CONSTANT, OP_STRING, OP_SCALAR, OP_ARRAY, OP_DB_INDEX,
};

// Messages

static const std::string op_name[] = {
 "add", "and", "arrow", "assign", "bar", "call", "constant",
 "divide", "endproc", "endprog", "equal", "fi", "greater",
 "index", "less", "minus", "modulo", "multiply", "not",
 "or", "proc", "prog", "read", "subtract", "value",
 "variable", "write", "block", "endblock", "char", "int", "float", "bool",
 "db_constant", "string", "scalar", "array", "double index",
};


// Convert any tags that need it to opcodes
const std::map<Tag, OpCode> tag_to_op{
  {AND, OP_AND},
  {OR, OP_OR},
  {NOT, OP_NOT},
  {EQ, OP_EQUAL},
  {LESS, OP_LESS},
  {GREATER, OP_GREATER},
  {PLUS, OP_ADD},
  {MINUS, OP_MINUS},
  {MULT, OP_MULTIPLY},
  {DIV, OP_DIVIDE},
  {MOD, OP_MODULO},
  {WRITE, OP_WRITE},
  {READ, OP_READ},
  {CHAR, OP_CHAR},
  {INT, OP_INT},
  {FLOAT, OP_FLOAT},
  {BOOL, OP_BOOL},
  {STRING, OP_STRING},
};

inline OpCode to_op(Tag t, bool sub = false) {
  if (sub)
    return OP_SUBTRACT;
  return tag_to_op.at(t);
}

static const int pow10[] {
  1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
};

const std::map<int, int> exp10 {
  {1,0},
  {10, 1},
  {100, 2},
  {1000, 3},
  {10000, 4},
  {100000, 5},
  {1000000, 6},
  {10000000, 7},
  {100000000, 8},
  {1000000000, 9},
};

}
#endif
