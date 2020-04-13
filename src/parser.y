%{
#include <stdio.h>
#include <string>
#include <iostream>
#include "Actions.h"

extern int line;
extern char* yytext;
extern "C" int yylex(void);

typedef yytokentype yytkn;

void yyerror(std::string, bool is_near = true);

Actions actions;
%}


%token BEG END
%token COMMA DOT SEMI
%token LHRND RHRND LHSQR RHSQR
%token WRITE ASGN IF DO ELIF ENDIF LOOP ENDLOOP SKIP
%token AND OR NOT
%token INIT EQ NEQ LESS GREATER LEQ GEQ
%token PLUS MINUS MULT DIV MOD
%token ARRAY PROC ENDPROC RECORD ENDREC TYPE SCALAR
%token INT BOOL FLOAT CHAR CONST
%token NUMBER TRUE FALSE NAME CHARACTER


%%
program:  /* nothing */
  | block DOT { printf("program\n\nTotal Lines: %d\n", line); }
  ;

block: BEG def_part stmt_part END { printf("block\n"); }
  ;


/* Definitions */
def_part: def_part def SEMI { printf("def_part\n"); }
  | def_part error SEMI { yyerrok; }
  | /* epsilon */
  ;

def: const_def { printf("def -> const_def\n\n"); }
  | var_def { printf("def -> var_def\n\n"); }
  ;

const_def: CONST type_sym name INIT constant { actions.const_def(line); printf("const_def\n"); }
  ;

var_def: type_sym v_prime { printf("var_def\n"); }
  ;

v_prime: var_list { actions.var_def(SCALAR, $$, line); printf("v_prime -> var_list\n"); }
  | ARRAY LHSQR constant RHSQR var_list { actions.array_def($5, line); printf("v_prime -> array\n"); }
  ;



/* Statements */
stmt_part: stmt_part stmt SEMI { printf("stmt_part\n"); }
  | stmt_part error SEMI { yyerrok; }
  | /* epsilon */
  ;

stmt: write_stmt { printf("stmt -> write\n\n"); }
  | asn_stmt { printf("stmt -> assign\n\n"); }
  | if_stmt { printf("stmt -> if\n\n"); }
  | loop_stmt { printf("stmt -> loop\n\n"); }
  | empty_stmt { printf("stmt -> empty\n\n"); }
  ;

write_stmt: WRITE expr_list { printf("write_stmt\n"); }
  ;

asn_stmt: var_access_list ASGN expr_list { actions.assign($1, $3, line); printf("assignment_stmt\n"); }
  ;

if_stmt: IF conditions ENDIF { printf("if_stmt\n"); }
  ;

loop_stmt: LOOP condition ENDLOOP { printf("loop_stmt\n"); }
  ;

conditions: conditions ELIF condition { printf("conditions\n"); }
  | condition
  ;

condition: expr DO stmt_part { printf("condition -> do\n"); }
  ;

empty_stmt: SKIP { printf("empty_stmt\n"); }
  ;


/* Expressions - All are going to be passing out nodes */
expr_list: expr_list COMMA expr { $$ = $1 + 1; printf("expr_list\n"); }
  | expr { $$ = 1; }
  | error { yyerrok; }
  ;

expr: expr prim_op prime_expr { printf("expr\n\n"); }
  | prime_expr
  ;

prime_expr: prime_expr rel_op simple_expr { printf("prim_expr\n"); }
  | simple_expr
  ;

simple_expr: simple_expr add_op t_prime { printf("simple_expr\n"); }
  | t_prime
  ;

t_prime: MINUS term { printf("t_prime -> MINUS\n"); }
  | term { printf("t_prime -> term\n"); }
  ;

term: factor mult_op factor
  | factor { printf("term\n"); }
  ;

/* factor should create or pass out a node */
factor: number { printf("factor -> number\n"); }
  | char { printf("factor -> char\n"); }
  | bool_sym { printf("factor -> bool_sym\n"); }
  | var_access { printf("factor -> var_access\n"); }
  | LHRND expr RHRND { printf("factor -> ( expr )\n"); }
  | NOT factor { actions.negate(line); printf("factor -> NOT\n"); }
  ;


/* Variables */
var_list: var_list COMMA name { $$ = $1 + 1; printf("var_list\n"); }
  | name { $$ = 1; printf("var_list -> name\n"); }
  ;

var_access_list: var_access_list COMMA var_access { $$ = $1 + 1; printf("var_access_list\n"); }
  | var_access { $$ = 1; }
  ;

var_access: name selector { actions.access(line, (yytkn)$2); printf("var_access\n"); }
  ;

selector: LHSQR expr RHSQR { $$ = ARRAY; printf("selector -> array access\n"); }
  | /* epsilon */ { $$ = SCALAR; }
  ;


/* Operators */
prim_op: AND { actions.add_t(yytkn::AND); printf("AND\n"); }
  | OR { actions.add_t(yytkn::OR); printf("OR\n"); }
  ;

rel_op: EQ { actions.add_t(yytkn::EQ); printf("EQUAL\n"); }
  | NEQ { actions.add_t(yytkn::NEQ); printf("NOT EQUAL\n"); }
  | LESS { actions.add_t(yytkn::LESS); printf("LESS\n"); }
  | GREATER { actions.add_t(yytkn::GREATER); printf("GREATER\n"); } 
  | LEQ { actions.add_t(yytkn::LEQ); printf("LESS EQUAL\n"); }
  | GEQ { actions.add_t(yytkn::GEQ); printf("GREATER EQUAL\n"); }
  ;

add_op: PLUS { actions.add_t(yytkn::PLUS); printf("PLUS\n"); }
  | MINUS { actions.add_t(yytkn::MINUS); printf("MINUS\n"); }
  ;

mult_op: MULT { actions.add_t(yytkn::MULT); printf("MULT\n"); }
  | DIV { actions.add_t(yytkn::DIV); printf("DIV\n"); }
  | MOD { actions.add_t(yytkn::MOD); printf("MOD\n"); }
  ;


/* Terminals */
type_sym: INT { actions.add_t(yytkn::INT); printf("INT\n"); }
  | FLOAT { actions.add_t(yytkn::FLOAT); printf("FLOAT\n"); }
  | BOOL { actions.add_t(yytkn::BOOL); printf("BOOL\n"); }
  | CHAR { actions.add_t(yytkn::CHAR); printf("CHAR\n"); }
  ;

constant: number { printf("constant -> number\n"); }
  | bool_sym { printf("constant -> bool\n"); } 
  | name { actions.constant(yytkn::NAME, line); printf("constant -> name\n"); }
  | char { printf("constant -> char\n"); }
  ;

char: CHARACTER { actions.add_c($1); actions.constant(yytkn::CHAR, line); printf("CHARACTER -> '%c'\n", $1); }
  ;

number: NUMBER DOT NUMBER { actions.add_f($1, $3); actions.constant(yytkn::FLOAT, line); printf("number -> float: %d.%d\n", $1, $3); }
  | NUMBER { actions.add_n($1); actions.constant(yytkn::INT, line); printf("number -> int: %d\n", $1); }
  ;

bool_sym: TRUE { actions.add_t(yytkn::TRUE); actions.constant(yytkn::BOOL, line); printf("FALSE\n"); }
  | FALSE { actions.add_t(yytkn::FALSE); actions.constant(yytkn::BOOL, line); printf("TRUE\n"); }
  ;

name: NAME { actions.add_w(std::string(yytext)); printf("NAME -> %s\n", yytext);}
  ;
%%


int main() {
  yyparse();

  // Print tokens to viusalize them
  actions.print_tokens();
  actions.print_nodes();
  actions.print_table();
}

void yyerror(std::string s, bool is_near) {
  std::cerr << "error on line " << line;
  if (is_near)
    std::cerr << " near '" << yytext << "'";
  std::cerr << ": " << s << std::endl;
}
