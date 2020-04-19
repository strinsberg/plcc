%{
#include <string>
#include <iostream>
#include <vector>
#include "Actions.h"
#include "AstNode.h"
#include "Symbol.h"
#include "Types.h"

extern char* yytext;
extern "C" int yylex(void);

void yyerror(std::string);
Actions* actions;
%}


%define api.value.type union
%type <int> NUMBER
%type <char> CHARACTER
%type <std::string*> name
%type <std::vector<std::string*>*> var_list
%type <std::pair<Expr*, std::vector<std::string*>*>*> vprime
%type <Def*> def_part def const_def var_def proc_def
%type <Expr*> expr prime_expr simple_expr term factor var_access
%type <Expr*> constant character number bool_sym tprime selector
%type <std::vector<Expr*>*> expr_list var_access_list
%type <Stmt*> program block bprime stmt_part stmt write_stmt read_stmt empty_stmt
%type <Stmt*> if_stmt loop_stmt proc_stmt block_stmt asn_stmt conditions condition
%type <Operator*> prim_op rel_op add_op mult_op
%type <Type*> type_sym

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
program:  /* nothing for bison */ { $$ = actions->empty_stmt(); }
  | block DOT { $$ = $1; }
  ;

block: BEG bprime END { $$ = $2; }
  ;

bprime: def_part stmt_part { $$ = actions->block($1, $2); }
  ;


/* Definitions */
def_part: def_part def SEMI { $$ = actions->def_part($1, $2); }
  | def_part error SEMI { $$ = $1; yyerrok; }
  | /* epsilon */ { $$ = actions->new_block(); }
  ;

def: const_def
  | var_def
  | proc_def
  ;

const_def: CONST type_sym name INIT constant { $$ = actions->const_def($2, $3, $5); }
  ;

var_def: type_sym vprime { $$ = actions->var_def($1, $2); }
  ;

vprime: var_list { $$ = actions->vprime($1); }
  | ARRAY LHSQR constant RHSQR var_list { $$ = actions->vprime($5, $3); }
  ;

proc_def: PROC name bprime ENDPROC { $$ = actions->proc_def($2, $3); }
  ;


/* Statements */
stmt_part: stmt_part stmt SEMI { $$ = actions->stmt_part($1, $2); }
  | stmt_part error SEMI { $$ = $1; yyerrok; }
  | /* epsilon */ { $$ = actions->empty_stmt(); }
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

write_stmt: WRITE expr_list { $$ = actions->io($2, symbol::WRITE); }
  ;

asn_stmt: var_access_list ASGN expr_list { $$ = actions->assign($1, $3); }
  ;

if_stmt: IF conditions ENDIF { $$ = actions->if_stmt($2); }
  ;

loop_stmt: LOOP condition ENDLOOP { $$ = actions->loop($2); }
  ;

conditions: conditions ELIF condition { $$ = actions->conditions($1, $3); }
  | condition { $$ = $1; }
  ;

condition: expr DO stmt_part { $$ = actions->condition($1, $3); }
  ;

empty_stmt: SKIP { $$ = actions->empty_stmt(); }
  ;

block_stmt: block { $$ = $1; }
  ;

proc_stmt: CALL name { $$ = actions->proc_stmt($2); }
  ;

read_stmt: READ expr_list { $$ = actions->io($2, symbol::READ); }
  ;

/* Expressions - All are going to be passing out nodes */
expr_list: expr_list COMMA expr { $$ = actions->expr_list($1, $3); }
  | expr { $$ = new std::vector<Expr*>{$1}; }
  | error { $$ = new std::vector<Expr*>(); yyerrok; }
  ;

/* Should all be nothing if only 1 and run a binary expr function if op expr */
expr: expr prim_op prime_expr { $$ = actions->binary($2, $1, $3); }
  | prime_expr { $$ = $1; }
  ;

prime_expr: prime_expr rel_op simple_expr { $$ = actions->binary($2, $1, $3); }
  | simple_expr { $$ = $1; }
  ;

simple_expr: simple_expr add_op tprime { $$ = actions->binary($2, $1, $3); }
  | tprime { $$ = $1; }
  ;

/* minus may need to be adjusted, this is a little ambiguous */
tprime: MINUS term { $$ = actions->unary(symbol::MINUS, $2); }
  | term { $$ = $1; }
  ;

term: factor mult_op factor { $$ = actions->binary($2, $1, $3); }
  | factor { $$ = $1; }
  ;

factor: number
  | character 
  | bool_sym 
  | var_access 
  | LHRND expr RHRND { $$ = $2; }
  | NOT factor { $$ = actions->unary(symbol::NOT, $2); }
  ;


/* Variables */
var_list: var_list COMMA name { $$ = actions->var_list($1, $3); }
  | name { $$ = new std::vector<std::string*>{$1}; }
  ;

var_access_list: var_access_list COMMA var_access {
      $$ = actions->expr_list($1, $3);
    }
  | var_access { $$ = new std::vector<Expr*>{$1}; }
  ;

var_access: name selector { $$ = actions->access($1, $2); }
  ;

selector: LHSQR expr RHSQR { $$ = $2; }
  | /* epsilon */ { $$ = actions->empty_expr(); }
  ;


/* Operators */
prim_op: AND { $$ = actions->new_op(symbol::AND, symbol::BOOL, symbol::BOOL); }
  | OR { $$ = actions->new_op(symbol::OR, symbol::BOOL, symbol::BOOL); }
  ;

rel_op: EQ { $$ = actions->new_op(symbol::EQ, symbol::UNIVERSAL, symbol::BOOL); }
  | NEQ { $$ = actions->new_op(symbol::NEQ, symbol::UNIVERSAL, symbol::BOOL); }
  | LESS { $$ = actions->new_op(symbol::LESS, symbol::NUMBER, symbol::BOOL); }
  | GREATER { $$ = actions->new_op(symbol::GREATER, symbol::NUMBER, symbol::BOOL); }
  | LEQ { $$ = actions->new_op(symbol::LEQ, symbol::NUMBER, symbol::BOOL); }
  | GEQ { $$ = actions->new_op(symbol::GEQ, symbol::NUMBER, symbol::BOOL); }
  ;

add_op: PLUS { $$ = actions->new_op(symbol::PLUS, symbol::NUMBER); }
  | MINUS { $$ = actions->new_op(symbol::MINUS, symbol::NUMBER); }
  ;

mult_op: MULT { $$ = actions->new_op(symbol::MULT, symbol::NUMBER); }
  | DIV { $$ = actions->new_op(symbol::DIV, symbol::NUMBER); }
  | MOD { $$ = actions->new_op(symbol::MOD, symbol::INT); }
  ;


/* Terminals */
type_sym: INT { $$ = actions->new_type(symbol::INT); }
  | FLOAT { $$ = actions->new_type(symbol::FLOAT); }
  | BOOL { $$ = actions->new_type(symbol::BOOL); }
  | CHAR { $$ = actions->new_type(symbol::CHAR); }
  ;

constant: number
  | bool_sym 
  | name { $$ = actions->access($1, actions->empty_expr()); }
  | character
  ;

character: CHARACTER { $$ = actions->constant(symbol::CHAR, $1); }
  ;

number: NUMBER DOT NUMBER { $$ = actions->constant(symbol::FLOAT, $1, $3); }
  | NUMBER { $$ = actions->constant(symbol::INT, $1); }
  ;

bool_sym: TRUE { $$ = actions->constant(symbol::TRUE, 1); }
  | FALSE { $$ = actions->constant(symbol::FALSE, 0); }
  ;

name: NAME { $$ = new std::string(yytext); }
  ;
%%


void yyerror(std::string s) {
  actions->get_admin()->error(s, yytext);
}
