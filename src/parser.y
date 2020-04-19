%{
#include <stdio.h>
#include <string>
#include <iostream>
#include "Actions.h"
#include "Symbol.h"
#include "Types.h"

extern char* yytext;
extern "C" int yylex(void);

void yyerror(std::string);
Actions* actions;
%}


%token BEG END
%token COMMA DOT SEMI
%token LHRND RHRND LHSQR RHSQR
%token WRITE ASGN IF DO ELIF ENDIF LOOP ENDLOOP SKIP CALL READ
%token AND OR NOT
%token INIT EQ NEQ LESS GREATER LEQ GEQ
%token PLUS MINUS MULT DIV MOD
%token ARRAY PROC ENDPROC RECORD ENDREC TYPE SCALAR
%token INT BOOL FLOAT CHAR CONST
%token NUMBER TRUE FALSE NAME CHARACTER
%token EMPTY NEWLINE


%%
program:  /* nothing for bison */
  | block DOT
  ;

block: BEG bprime END 
  ;

bprime: def_part stmt_part { actions->block($1, $2); }
  ;


/* Definitions */
def_part: def_part def SEMI {
      $$ = $1 + 1;
      actions->get_admin()->debug("def_part\n");
    }
  | def_part error SEMI { $$ = $1; yyerrok; }
  | /* epsilon */ { $$ = 0; actions->new_block(); }
  ;

def: const_def
  | var_def
  | proc_def
  ;

const_def: CONST type_sym name INIT constant { actions->const_def(); }
  ;

var_def: type_sym v_prime
  ;

v_prime: var_list { actions->var_def($$); }
  | ARRAY LHSQR constant RHSQR var_list { actions->array_def($5); }
  ;

proc_def: PROC name { actions->proc_name(); }
          bprime ENDPROC { actions->proc_def(); }
  ;


/* Statements */
stmt_part: stmt_part stmt SEMI {
      $$ = $1 + 1;
      actions->get_admin()->debug("stmt_part\n");
    }
  | stmt_part error SEMI { $$ = $1; yyerrok; }
  | /* epsilon */ { $$ = 0; }
  ;

stmt: write_stmt
  | asn_stmt
  | if_stmt
  | loop_stmt
  | empty_stmt
  | block_stmt
  | proc_stmt
  | read_stmt
  ;

write_stmt: WRITE expr_list { actions->io($2, symbol::WRITE); }
  ;

asn_stmt: var_access_list ASGN expr_list { actions->assign($1, $3);; }
  ;

if_stmt: IF conditions ENDIF { actions->if_stmt($2); }
  ;

loop_stmt: LOOP condition ENDLOOP { actions->loop(); }
  ;

conditions: conditions ELIF condition {
      $$ = $1 + 1;
      actions->get_admin()->debug("conditions");
    }
  | condition { $$ = 1; }
  ;

condition: expr DO stmt_part { actions->condition($3); }
  ;

empty_stmt: SKIP { actions->empty(); }
  ;

block_stmt: block 
  ;

proc_stmt: CALL name { actions->proc_stmt(); }
  ;

read_stmt: READ expr_list { actions->io($2, symbol::READ); }
  ;

/* Expressions - All are going to be passing out nodes */
expr_list: expr_list COMMA expr {
      $$ = $1 + 1;
      actions->get_admin()->debug("expr_list");
    }
  | expr { $$ = 1; }
  | error { $$ = 0; yyerrok; }
  ;

/* Should all be nothing if only 1 and run a binary expr function if op expr */
expr: expr prim_op prime_expr { actions->binary(); }
  | prime_expr
  ;

prime_expr: prime_expr rel_op simple_expr { actions->binary(); }
  | simple_expr
  ;

simple_expr: simple_expr add_op t_prime { actions->binary(); }
  | t_prime
  ;

/* minus may need to be adjusted, this is a little ambiguous */
t_prime: MINUS term {
      actions->new_op(symbol::MINUS, symbol::NUMBER);
      actions->unary();
    } 
  | term
  ;

term: factor mult_op factor { actions->binary(); }
  | factor
  ;

factor: number
  | char 
  | bool_sym 
  | var_access 
  | LHRND expr RHRND
  | NOT factor {
      actions->new_op(symbol::NOT, symbol::BOOL);
      actions->unary();
    }
  ;


/* Variables */
var_list: var_list COMMA name {
      $$ = $1 + 1;
      actions->get_admin()->debug("var_list");
    }
  | name {
      $$ = 1;
      actions->get_admin()->debug("var_list -> name");
    }
  ;

var_access_list: var_access_list COMMA var_access {
      $$ = $1 + 1;
      actions->get_admin()->debug("var_access_list");
    }
  | var_access { $$ = 1; }
  ;

var_access: name selector { actions->access((symbol::Tag)$2); }
  ;

selector: LHSQR expr RHSQR {
      $$ = (int)symbol::ARRAY;
      actions->get_admin()->debug("selector -> array access");
    }
  | /* epsilon */ { $$ = (int)symbol::SCALAR; }
  ;


/* Operators */
prim_op: AND { actions->new_op(symbol::AND, symbol::BOOL, symbol::BOOL); }
  | OR { actions->new_op(symbol::OR, symbol::BOOL, symbol::BOOL); }
  ;

rel_op: EQ { actions->new_op(symbol::EQ, symbol::UNIVERSAL, symbol::BOOL); }
  | NEQ { actions->new_op(symbol::NEQ, symbol::UNIVERSAL, symbol::BOOL); }
  | LESS { actions->new_op(symbol::LESS, symbol::NUMBER, symbol::BOOL); }
  | GREATER { actions->new_op(symbol::GREATER, symbol::NUMBER, symbol::BOOL); }
  | LEQ { actions->new_op(symbol::LEQ, symbol::NUMBER, symbol::BOOL); }
  | GEQ { actions->new_op(symbol::GEQ, symbol::NUMBER, symbol::BOOL); }
  ;

add_op: PLUS { actions->new_op(symbol::PLUS, symbol::NUMBER); }
  | MINUS { actions->new_op(symbol::MINUS, symbol::NUMBER); }
  ;

mult_op: MULT { actions->new_op(symbol::MULT, symbol::NUMBER); }
  | DIV { actions->new_op(symbol::DIV, symbol::NUMBER); }
  | MOD { actions->new_op(symbol::MOD, symbol::INT); }
  ;


/* Terminals */
type_sym: INT { actions->new_type(symbol::INT); }
  | FLOAT { actions->new_type(symbol::FLOAT); }
  | BOOL { actions->new_type(symbol::BOOL); }
  | CHAR { actions->new_type(symbol::CHAR); }
  ;

constant: number
  | bool_sym 
  | name { actions->constant(symbol::NAME); }
  | char
  ;

char: CHARACTER { actions->constant(symbol::CHAR, $1); }
  ;

number: NUMBER DOT NUMBER { actions->constant(symbol::FLOAT, $1, $3); }
  | NUMBER { actions->constant(symbol::INT, $1); }
  ;

bool_sym: TRUE { actions->constant(symbol::TRUE, 1); }
  | FALSE { actions->constant(symbol::FALSE, 0); }
  ;

name: NAME {
      actions->new_name(std::string(yytext));
      actions->get_admin()->debug("NAME -> " + std::string(yytext));
    }
  ;
%%


void yyerror(std::string s) {
  actions->get_admin()->error(s, yytext);
}
