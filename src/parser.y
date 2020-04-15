%{
#include <stdio.h>
#include <string>
#include <iostream>
#include "Actions.h"
#include "Symbol.h"

extern int line; // Change to returning newline and parsing it?
extern char* yytext;
extern "C" int yylex(void);

// Create an error function and call it with this so that this stays out of the
// C++ parts of the code.
void yyerror(std::string);

// Make sure that the parser stays separate from the other parts since it is
// Still using C code. In the future maybe expose these through another
// object that can bridge between them. Unless that bridge is already the action
// class, It will have the AST in it when things are done.
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
%token EMPTY


%%
program:  /* nothing */
  | block DOT { printf("program\n\nTotal Lines: %d\n", line); }
  ;

block: BEG def_part stmt_part END { actions.block($2, $3, line); printf("block\n"); }
  ;


/* Definitions */
def_part: def_part def SEMI { $$ = $1 + 1; printf("def_part\n\n"); }
  | def_part error SEMI { $$ = $1; yyerrok; }
  | /* epsilon */ { $$ = 0; actions.new_block(); }
  ;

def: const_def { printf("def -> const_def\n"); }
  | var_def { printf("def -> var_def\n"); }
  ;

const_def: CONST type_sym name INIT constant { actions.const_def(line); printf("const_def\n"); }
  ;

var_def: type_sym v_prime { printf("var_def\n"); }
  ;

v_prime: var_list { actions.var_def(tag::SCALAR, $$, line); printf("v_prime -> var_list\n"); }
  | ARRAY LHSQR constant RHSQR var_list { actions.array_def($5, line); printf("v_prime -> array\n"); }
  ;



/* Statements */
stmt_part: stmt_part stmt SEMI { $$ = $1 + 1; printf("stmt_part\n\n"); }
  | stmt_part error SEMI { $$ = $1; yyerrok; }
  | /* epsilon */ { $$ = 0; }
  ;

stmt: write_stmt { printf("stmt -> write\n"); }
  | asn_stmt { printf("stmt -> assign\n"); }
  | if_stmt { printf("stmt -> if\n"); }
  | loop_stmt { printf("stmt -> loop\n"); }
  | empty_stmt { printf("stmt -> empty\n"); }
  | block_stmt { printf("stmt -> block\n"); }
  ;

write_stmt: WRITE expr_list { actions.write($2, line); printf("write_stmt\n"); }
  ;

asn_stmt: var_access_list ASGN expr_list { actions.assign($1, $3, line); printf("assignment_stmt\n"); }
  ;

if_stmt: IF conditions ENDIF { actions.if_stmt($2, line); printf("if_stmt\n"); }
  ;

loop_stmt: LOOP condition ENDLOOP { actions.loop(line); printf("loop_stmt\n"); }
  ;

conditions: conditions ELIF condition { $$ = $1 + 1; printf("conditions\n"); }
  | condition { $$ = 1; }
  ;

condition: expr DO stmt_part { actions.condition($3, line); printf("condition -> do\n"); }
  ;

empty_stmt: SKIP { actions.empty(line); printf("empty_stmt\n"); }
  ;

block_stmt: block { printf("block_stmt\n"); }


/* Expressions - All are going to be passing out nodes */
expr_list: expr_list COMMA expr { $$ = $1 + 1; printf("expr_list\n"); }
  | expr { $$ = 1; }
  | error { $$ = 0; yyerrok; }
  ;

/* Should all be nothing if only 1 and run a binary expr function if op expr */
expr: expr prim_op prime_expr { actions.binary(line); printf("expr\n\n"); }
  | prime_expr
  ;

prime_expr: prime_expr rel_op simple_expr { actions.binary(line); printf("prim_expr\n"); }
  | simple_expr
  ;

simple_expr: simple_expr add_op t_prime { actions.binary(line); printf("simple_expr\n"); }
  | t_prime
  ;

/* pop top and return unary with minus if first rule */
t_prime: MINUS term { actions.unary(tag::MINUS, line); printf("t_prime -> MINUS\n"); }
  | term { printf("t_prime -> term\n"); }
  ;

/* Like expr for adding a binary expr if an op is found */
term: factor mult_op factor { actions.binary(line); }
  | factor { printf("term\n"); }
  ;

/* All can just leave the expression on the stack. except not needs to put in unary */
factor: number { printf("factor -> number\n"); }
  | char { printf("factor -> char\n"); }
  | bool_sym { printf("factor -> bool_sym\n"); }
  | var_access { printf("factor -> var_access\n"); }
  | LHRND expr RHRND { printf("factor -> ( expr )\n"); }
  | NOT factor { actions.unary(tag::NOT, line); printf("factor -> NOT\n"); }
  ;


/* Variables */
var_list: var_list COMMA name { $$ = $1 + 1; printf("var_list\n"); }
  | name { $$ = 1; printf("var_list -> name\n"); }
  ;

var_access_list: var_access_list COMMA var_access { $$ = $1 + 1; printf("var_access_list\n"); }
  | var_access { $$ = 1; }
  ;

var_access: name selector { actions.access(line, (tag::Tag)$2); printf("var_access\n"); }
  ;

selector: LHSQR expr RHSQR { $$ = (int)tag::ARRAY; printf("selector -> array access\n"); }
  | /* epsilon */ { $$ = int(tag::SCALAR); }
  ;


/* Operators */
prim_op: AND { actions.add_t(tag::AND); printf("AND\n"); }
  | OR { actions.add_t(tag::OR); printf("OR\n"); }
  ;

rel_op: EQ { actions.add_t(tag::EQ); printf("EQUAL\n"); }
  | NEQ { actions.add_t(tag::NEQ); printf("NOT EQUAL\n"); }
  | LESS { actions.add_t(tag::LESS); printf("LESS\n"); }
  | GREATER { actions.add_t(tag::GREATER); printf("GREATER\n"); } 
  | LEQ { actions.add_t(tag::LEQ); printf("LESS EQUAL\n"); }
  | GEQ { actions.add_t(tag::GEQ); printf("GREATER EQUAL\n"); }
  ;

add_op: PLUS { actions.add_t(tag::PLUS); printf("PLUS\n"); }
  | MINUS { actions.add_t(tag::MINUS); printf("MINUS\n"); }
  ;

mult_op: MULT { actions.add_t(tag::MULT); printf("MULT\n"); }
  | DIV { actions.add_t(tag::DIV); printf("DIV\n"); }
  | MOD { actions.add_t(tag::MOD); printf("MOD\n"); }
  ;


/* Terminals */
type_sym: INT { actions.add_t(tag::INT); printf("INT\n"); }
  | FLOAT { actions.add_t(tag::FLOAT); printf("FLOAT\n"); }
  | BOOL { actions.add_t(tag::BOOL); printf("BOOL\n"); }
  | CHAR { actions.add_t(tag::CHAR); printf("CHAR\n"); }
  ;

constant: number { printf("constant -> number\n"); }
  | bool_sym { printf("constant -> bool\n"); } 
  | name { actions.constant(tag::NAME, line); printf("constant -> name\n"); }
  | char { printf("constant -> char\n"); }
  ;

char: CHARACTER { actions.add_c($1); actions.constant(tag::CHAR, line); printf("CHARACTER -> '%c'\n", $1); }
  ;

number: NUMBER DOT NUMBER { actions.add_f($1, $3); actions.constant(tag::FLOAT, line); printf("number -> float: %d.%d\n", $1, $3); }
  | NUMBER { actions.add_n($1); actions.constant(tag::INT, line); printf("number -> int: %d\n", $1); }
  ;

bool_sym: TRUE { actions.add_t(tag::TRUE); actions.constant(tag::BOOL, line); printf("FALSE\n"); }
  | FALSE { actions.add_t(tag::FALSE); actions.constant(tag::BOOL, line); printf("TRUE\n"); }
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

void yyerror(std::string s) {
  actions.error(s, line, yytext);
}
