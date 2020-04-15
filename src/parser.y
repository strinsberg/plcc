%{
#include <stdio.h>
#include <string>
#include <iostream>
#include "Actions.h"
#include "Symbol.h"

extern char* yytext;
extern "C" int yylex(void);

void yyerror(std::string);
Actions* actions;
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
%token EMPTY NEWLINE


%%
program:  /* nothing */
  | block DOT { printf("program\n\nTotal Lines: %d\n", actions->line()); }
  ;

block: BEG def_part stmt_part END { actions->block($2, $3); printf("block\n"); }
  ;


/* Definitions */
def_part: def_part def SEMI { $$ = $1 + 1; printf("def_part\n\n"); }
  | def_part error SEMI { $$ = $1; yyerrok; }
  | /* epsilon */ { $$ = 0; actions->new_block(); }
  ;

def: const_def { printf("def -> const_def\n"); }
  | var_def { printf("def -> var_def\n"); }
  ;

const_def: CONST type_sym name INIT constant { actions->const_def(); printf("const_def\n"); }
  ;

var_def: type_sym v_prime { printf("var_def\n"); }
  ;

v_prime: var_list { actions->var_def(tag::SCALAR, $$); printf("v_prime -> var_list\n"); }
  | ARRAY LHSQR constant RHSQR var_list { actions->array_def($5); printf("v_prime -> array\n"); }
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

write_stmt: WRITE expr_list { actions->write($2); printf("write_stmt\n"); }
  ;

asn_stmt: var_access_list ASGN expr_list { actions->assign($1, $3); printf("assignment_stmt\n"); }
  ;

if_stmt: IF conditions ENDIF { actions->if_stmt($2); printf("if_stmt\n"); }
  ;

loop_stmt: LOOP condition ENDLOOP { actions->loop(); printf("loop_stmt\n"); }
  ;

conditions: conditions ELIF condition { $$ = $1 + 1; printf("conditions\n"); }
  | condition { $$ = 1; }
  ;

condition: expr DO stmt_part { actions->condition($3); printf("condition -> do\n"); }
  ;

empty_stmt: SKIP { actions->empty(); printf("empty_stmt\n"); }
  ;

block_stmt: block { printf("block_stmt\n"); }


/* Expressions - All are going to be passing out nodes */
expr_list: expr_list COMMA expr { $$ = $1 + 1; printf("expr_list\n"); }
  | expr { $$ = 1; }
  | error { $$ = 0; yyerrok; }
  ;

/* Should all be nothing if only 1 and run a binary expr function if op expr */
expr: expr prim_op prime_expr { actions->binary(); printf("expr\n\n"); }
  | prime_expr
  ;

prime_expr: prime_expr rel_op simple_expr { actions->binary(); printf("prim_expr\n"); }
  | simple_expr
  ;

simple_expr: simple_expr add_op t_prime { actions->binary(); printf("simple_expr\n"); }
  | t_prime
  ;

/* pop top and return unary with minus if first rule */
t_prime: MINUS term { actions->unary(tag::MINUS); printf("t_prime -> MINUS\n"); }
  | term { printf("t_prime -> term\n"); }
  ;

/* Like expr for adding a binary expr if an op is found */
term: factor mult_op factor { actions->binary(); }
  | factor { printf("term\n"); }
  ;

/* All can just leave the expression on the stack. except not needs to put in unary */
factor: number { printf("factor -> number\n"); }
  | char { printf("factor -> char\n"); }
  | bool_sym { printf("factor -> bool_sym\n"); }
  | var_access { printf("factor -> var_access\n"); }
  | LHRND expr RHRND { printf("factor -> ( expr )\n"); }
  | NOT factor { actions->unary(tag::NOT); printf("factor -> NOT\n"); }
  ;


/* Variables */
var_list: var_list COMMA name { $$ = $1 + 1; printf("var_list\n"); }
  | name { $$ = 1; printf("var_list -> name\n"); }
  ;

var_access_list: var_access_list COMMA var_access { $$ = $1 + 1; printf("var_access_list\n"); }
  | var_access { $$ = 1; }
  ;

var_access: name selector { actions->access((tag::Tag)$2); printf("var_access\n"); }
  ;

selector: LHSQR expr RHSQR { $$ = (int)tag::ARRAY; printf("selector -> array access\n"); }
  | /* epsilon */ { $$ = int(tag::SCALAR); }
  ;


/* Operators */
prim_op: AND { actions->new_token(tag::AND); printf("AND\n"); }
  | OR { actions->new_token(tag::OR); printf("OR\n"); }
  ;

rel_op: EQ { actions->new_token(tag::EQ); printf("EQUAL\n"); }
  | NEQ { actions->new_token(tag::NEQ); printf("NOT EQUAL\n"); }
  | LESS { actions->new_token(tag::LESS); printf("LESS\n"); }
  | GREATER { actions->new_token(tag::GREATER); printf("GREATER\n"); } 
  | LEQ { actions->new_token(tag::LEQ); printf("LESS EQUAL\n"); }
  | GEQ { actions->new_token(tag::GEQ); printf("GREATER EQUAL\n"); }
  ;

add_op: PLUS { actions->new_token(tag::PLUS); printf("PLUS\n"); }
  | MINUS { actions->new_token(tag::MINUS); printf("MINUS\n"); }
  ;

mult_op: MULT { actions->new_token(tag::MULT); printf("MULT\n"); }
  | DIV { actions->new_token(tag::DIV); printf("DIV\n"); }
  | MOD { actions->new_token(tag::MOD); printf("MOD\n"); }
  ;


/* Terminals */
type_sym: INT { actions->new_token(tag::INT); printf("INT\n"); }
  | FLOAT { actions->new_token(tag::FLOAT); printf("FLOAT\n"); }
  | BOOL { actions->new_token(tag::BOOL); printf("BOOL\n"); }
  | CHAR { actions->new_token(tag::CHAR); printf("CHAR\n"); }
  ;

constant: number { printf("constant -> number\n"); }
  | bool_sym { printf("constant -> bool\n"); } 
  | name { actions->constant(tag::NAME); printf("constant -> name\n"); }
  | char { printf("constant -> char\n"); }
  ;

char: CHARACTER { actions->constant(tag::CHAR, $1); printf("CHARACTER -> '%c'\n", $1); }
  ;

number: NUMBER DOT NUMBER { actions->constant(tag::FLOAT, $1, $3); printf("number -> float: %d.%d\n", $1, $3); }
  | NUMBER { actions->constant(tag::INT, $1); printf("number -> int: %d\n", $1); }
  ;

bool_sym: TRUE { actions->constant(tag::TRUE); printf("FALSE\n"); }
  | FALSE { actions->constant(tag::FALSE); printf("TRUE\n"); }
  ;

name: NAME { actions->new_token(tag::NAME, std::string(yytext)); printf("NAME -> %s\n", yytext);}
  ;
%%


void yyerror(std::string s) {
  actions->error(s, yytext);
}
