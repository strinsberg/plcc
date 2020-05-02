%{
#include <cstdio>
#include <string>
#include <vector>
#include <memory>
#include "Actions.h"
#include "AstNode.h"
#include "Symbol.h"
#include "Types.h"

// From scanner.l
extern char* yytext;
extern int yylex();
extern FILE* yyin;
bool set_file(std::string);
%}


%language "c++"

/* Non-terminal type definitions */
%define api.value.type variant
%type <int> num
%type <std::string> name
%type <std::vector<std::string>> var_list

%type <Vars> vprime
%type <std::shared_ptr<DefPart>> var_def
%type <std::shared_ptr<Def>> const_def proc_def

%type <std::shared_ptr<Expr>> expr prime_expr simple_expr term 
%type <std::shared_ptr<Expr>> factor var_access selector endl
%type <std::shared_ptr<Expr>> constant character number bool_sym string
%type <std::vector<std::shared_ptr<Expr>>> expr_list var_access_list

%type <std::shared_ptr<Stmt>> stmt
%type <std::shared_ptr<Stmt>> write_stmt read_stmt empty_stmt
%type <std::shared_ptr<Stmt>> if_stmt loop_stmt proc_stmt
%type <std::shared_ptr<Stmt>> asn_stmt conditions then_cond do_cond

%type <Operator> prim_op rel_op add_op mult_op
%type <Type> type_sym type


/* Token definitions */
%token BEG END INCLUDES MAIN MODULE END_MAIN END_INC
%token COMMA DOT SEMI COLON DB_COLON ARROW
%token LHRND RHRND LHSQR RHSQR LHCUR RHCUR
%token WRITE ASGN IF THEN ELIF ENDIF LOOP DO ENDLOOP SKIP CALL READ READLN
%token AND OR NOT
%token INIT EQ NEQ LESS GREATER LEQ GEQ
%token PLUS MINUS MULT DIV MOD
%token ARRAY PROC ENDPROC RECORD ENDREC TYPE SCALAR VECTOR MAP REF
%token INT BOOL FLOAT CHAR CONST
%token NUMBER TRUE FALSE NAME CHARACTER
%token EMPTY NEWLINE STRING AS


/* Code for C++ yylex and yyerror definitions */
%code {

/* Actions object to perform actions and allow access to admin.
   The actual object is created and deleted by the Parser class */
std::shared_ptr<Actions> actions;

/* Wrapper function to call scanners yylex function */
int my_lex() { return yylex(); }

namespace yy {
  /* Temp variables to hold actual number and char values */
  static int temp_num = 0;
  static char temp_ch = '\0';
 
  /* C++ lexing function that handles numbers and char values */
  int yylex(parser::semantic_type* t) {
    int token = my_lex();

    if (token == parser::token::NUMBER)
      temp_num = atoi(yytext);
    else if (token == parser::token::CHARACTER)
      temp_ch = yytext[1];

    return token;
  }

  /* C++ error function calls admin::error */
  void parser::error(const std::string& s) {
    actions->get_admin()->error(s, yytext);
  }
}
}


%%

/* Module *******************************************************************/
module: MODULE name includes mod_defs { printf("\nend module\n"); }
  | program {}
  ;

program: includes main { printf("\nend no defs program\n"); }
  | includes mod_defs main { printf("\nend program\n"); }
  ;


/* Includes *****************************************************************/
includes: INCLUDES include_stmts END_INC { printf("\nincludes\n"); }
  | /* epsilon */ { printf("\nempty includes\n"); }
  ;

include_stmts: include_stmts include {}
  | include {} 
  ;

include: string AS name SEMI {}
  | string SEMI {}
  ;


/* Module Definitions *******************************************************/
mod_defs: mod_defs mod_def SEMI { }
  | mod_def SEMI {}
  ;

mod_def: const_def {}
  | type_def {}
  | proc_def {}
  ;

const_def: CONST const_type name INIT constant { }
  ;

type_def: RECORD name var_defs ENDREC {}
  ;

proc_def: PROC name params body ENDPROC {}
  ;

params: LHRND param_list RHRND {}
  | /* epsilon */
  ;

param_list: param_list SEMI param {}
  | param {}
  ;

param: pass_by var_def {}
  ;

pass_by: REF | CONST | /* epsilon */ {}
  ;

/* Program Body *************************************************************/
main: MAIN body END_MAIN { printf("\nmain body\n"); }
  ;

body: body body_stmt SEMI {}
  | body_stmt SEMI {}
  ;

body_stmt: var_def {}
  | stmt {}
  ;


/* Constants ****************************************************************/
const_type: type ARRAY {}
  | type VECTOR {}
  | type ARROW type MAP {}
  | type {}
  ;

constant_list: constant_list COMMA constant {}
  | constant {}
  ;

constant: name { }
  | literal { }
  ;

literal: array_lit {}
  | map_lit {}
  | number {}
  | bool_sym {}
  | character {}
  ;

array_lit: LHSQR constant_list RHSQR {}
  ;

map_lit: LHCUR const_mapping_list RHCUR {}
  ;

const_mapping_list: const_mapping_list COMMA const_mapping {}
  | const_mapping {}
  ;

const_mapping: constant COLON constant {}
  ; 



/* Variable Definitions *****************************************************/
var_defs: var_defs var_def SEMI {}
  | var_def SEMI {}
  ;

var_def: type vprime { }
  | type vprime INIT expr_list {}
  ;

type: type_sym { }
  | name { }
  ;

vprime: var_list { }
  | ARRAY LHSQR constant RHSQR var_list { }
  ;


/* Statements */
stmt: write_stmt { }
  | asn_stmt { }
  | if_stmt { }
  | loop_stmt { }
  | empty_stmt { }
  | proc_stmt { }
  | read_stmt { }
  ;

write_stmt: WRITE expr_list { }
  ;

asn_stmt: var_access_list ASGN expr_list { }
  ;

if_stmt: IF conditions ENDIF { }
  ;

conditions: conditions ELIF then_cond { }
  | then_cond { }
  ;

then_cond: expr THEN body { }
  ;

loop_stmt: LOOP do_cond ENDLOOP { }
  ;

do_cond: expr DO body { }
  ;

empty_stmt: SKIP { }
  ;

proc_stmt: CALL name args { }
  | name DB_COLON name args {}
  ;

args: LHRND expr_list RHRND {}
  | LHRND RHRND {}
  | /* epsilon */
  ;

read_stmt: READ var_access_list { }
  | READLN var_access { }
  ;


/* Expressions */
expr_list: expr_list COMMA expr { }
  | expr { }
  | error { }
  ;

expr: expr prim_op prime_expr { }
  | prime_expr { }
  ;

prime_expr: prime_expr rel_op simple_expr { }
  | simple_expr { }
  ;

simple_expr: simple_expr add_op term { }
  | term { }
  ;

term: factor mult_op factor { }
  | factor { }
  ;

factor: number { }
  | character  { }
  | bool_sym  { }
  | string { }
  | endl { }
  | var_access  { }
  | LHRND expr RHRND { }
  | NOT factor { }
  | MINUS factor { }
  ;


/* Variables */
var_list: var_list COMMA name { }
  | name { }
  ;

var_access_list: var_access_list COMMA var_access { }
  | var_access { }
  ;

var_access: name selector { }
  | var_access DOT name selector { }
  ;

selector: LHSQR expr RHSQR { }
  | /* epsilon */
    { }
  ;


/* Operators Terminals */
prim_op: AND { }
  | OR { }
  ;

rel_op: EQ { }
  | NEQ { }
  | LESS { }
  | GREATER { }
  | LEQ { }
  | GEQ { }
  ;

add_op: PLUS { }
  | MINUS { }
  ;

mult_op: MULT { }
  | DIV { }
  | MOD { }
  ;


/* Value Based Terminals */
type_sym: INT { }
  | FLOAT { }
  | BOOL { }
  | CHAR { }
  ;


character: CHARACTER { }
  ;

number: num DOT NUMBER { }
  | num { }
  ;

num: NUMBER { }
  ;

bool_sym: TRUE { }
  | FALSE { }
  ;

string: STRING { }
  ;

endl: NEWLINE { }
  ;

name: NAME { }
  ;
%%

