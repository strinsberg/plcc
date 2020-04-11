#ifndef PLCC_SYMBOL_H
#define PLCC_SYMBOL_H

#include "parser.tab.h"
#include <string>
#include <map>


const std::map<yytokentype,std::string> tok_string {
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
};


#endif
