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
  EMPTY, CALL, READ, UNIVERSAL,
};


const std::map<Tag, std::string> to_string {
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


// To simplify to string calls. Not sure inline is good practice, but
// something is necessary to define it here without a source file.
inline std::string str(Tag t) {
  return to_string.at(t);
}

}
#endif
