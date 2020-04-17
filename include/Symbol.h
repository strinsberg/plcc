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
  INT, BOOL, FLOAT, CHAR, CONST,
  NUMBER, TRUE, FALSE, NAME, CHARACTER,
  EMPTY, CALL, READ, UNIVERSAL, NUMERIC,
};


const std::map<Tag, std::string> to_string {
  {BEG, "BEGIN"},
  {END, "END"},
  {COMMA, "COMMA"},
  {DOT, "DOT"},
  {SEMI, "SEMI"},
  {LHRND, "LHRND"},
  {RHRND, "RHRND"},
  {LHSQR, "LHSQR"},
  {RHSQR, "RHSQR"},
  {WRITE, "WRITE"},
  {ASGN, "ASGN"},
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
  {INIT, "INIT"},
  {EQ, "EQ"},
  {NEQ, "NEQ"},
  {LESS, "LESS"},
  {GREATER, "GREATER"},
  {LEQ, "LEQ"},
  {GEQ, "GEQ"},
  {PLUS, "PLUS"},
  {MINUS, "MINUS"},
  {MULT, "MULT"},
  {DIV, "DIV"},
  {MOD, "MOD"},
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
};

}
#endif
