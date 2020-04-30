%{
#include <cstdio>
#include <string>
#include <iostream>
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
%type <std::shared_ptr<DefPart>> def_part var_def params param_list param
%type <std::shared_ptr<Def>> def const_def proc_def rec_def

%type <std::shared_ptr<Id>> rec_name proc_name

%type <std::shared_ptr<Expr>> expr prime_expr simple_expr term 
%type <std::shared_ptr<Expr>> factor var_access selector endl
%type <std::shared_ptr<Expr>> constant character number bool_sym string
%type <std::vector<std::shared_ptr<Expr>>> expr_list var_access_list proc_args

%type <std::shared_ptr<Block>> bprime block
%type <std::shared_ptr<BlockStmt>> block_stmt
%type <std::shared_ptr<Stmt>> program stmt_part stmt
%type <std::shared_ptr<Stmt>> write_stmt read_stmt empty_stmt
%type <std::shared_ptr<Stmt>> if_stmt loop_stmt proc_stmt
%type <std::shared_ptr<Stmt>> asn_stmt conditions then_cond do_cond

%type <Operator> prim_op rel_op add_op mult_op
%type <Type> type_sym type


/* Token definitions */
%token BEG END
%token COMMA DOT SEMI
%token LHRND RHRND LHSQR RHSQR
%token WRITE ASGN IF THEN ELIF ENDIF LOOP DO ENDLOOP SKIP CALL READ READLN
%token AND OR NOT
%token INIT EQ NEQ LESS GREATER LEQ GEQ
%token PLUS MINUS MULT DIV MOD
%token ARRAY PROC ENDPROC RECORD ENDREC TYPE SCALAR REF VAL
%token INT BOOL FLOAT CHAR CONST
%token NUMBER TRUE FALSE NAME CHARACTER
%token EMPTY NEWLINE STRING


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
program: block DOT { actions->set_ast($1); }
  ;

block: BEG bprime END { $$ = $2; }
  ;

bprime: def_part stmt_part { $$ = actions->block($1, $2); }
  ;


/* Definitions */
def_part: def_part def SEMI { $1->add_def($2); $$ = $1; }
  | def_part var_def SEMI { $1->add_defs($2); $$ = $1; }
  | def_part error SEMI { $$ = $1; yyerrok; }
  | /* epsilon */ { $$ = actions->new_block(); }
  ;

def: const_def { $$ = $1; }
  | proc_def { $$ = $1; }
  | rec_def { $$ = $1; }
  ;

const_def: CONST type_sym name INIT constant { $$ = actions->const_def($2, $3, $5); }
  ;

var_def: type vprime { $$ = actions->var_def($1, $2); }
  ;

type: type_sym { $$ = $1; }
  | TYPE name { $$ = actions->new_type(symbol::RECORD, $2); }
  ;

vprime: var_list { $$ = actions->vprime($1); }
  | ARRAY LHSQR constant RHSQR var_list { $$ = actions->vprime($5, $3); }
  ;

proc_def: PROC proc_name params bprime ENDPROC { $$ = actions->proc_def($2, $3, $4); }
  ;

proc_name: name { $$ = actions->proc_name($1); }
  ;

params: LHRND param_list RHRND { $$ = $2; }
  | /* epsilon */ { $$ = std::make_shared<DefPart>(); }
  ;

param_list: param_list SEMI param { $1->add_defs($3); $$ = $1; }
  | param { $$ = $1; }
  ;

param: REF type vprime { $2.qual = symbol::REF_PARAM; $$ = actions->var_def($2, $3); }
  | VAL type vprime { $2.qual = symbol::VAL_PARAM; $$ = actions->var_def($2, $3); }
  ;

rec_def: rec_name def_part ENDREC { $$ = actions->rec_def($1, $2); }
  ;

rec_name: RECORD name { $$ = actions->rec_name($2); }
  ;


/* Statements */
stmt_part: stmt_part stmt SEMI { $$ = actions->stmt_part($1, $2); }
  | stmt_part error SEMI { $$ = $1; yyerrok; }
  | /* epsilon */
    { auto stmt = actions->empty_stmt(); stmt->set_null(true); $$ = stmt; }
  ;

stmt: write_stmt { $$ = $1; }
  | asn_stmt { $$ = $1; }
  | if_stmt { $$ = $1; }
  | loop_stmt { $$ = $1; }
  | empty_stmt { $$ = $1; }
  | block_stmt { $$ = $1; }
  | proc_stmt { $$ = $1; }
  | read_stmt { $$ = $1; }
  ;

write_stmt: WRITE expr_list { $$ = actions->io($2, symbol::WRITE); }
  ;

asn_stmt: var_access_list ASGN expr_list { $$ = actions->assign($1, $3); }
  ;

if_stmt: IF conditions ENDIF { $$ = actions->if_stmt($2); }
  ;

conditions: conditions ELIF then_cond { $$ = actions->conditions($1, $3); }
  | then_cond { $$ = $1; }
  ;

then_cond: expr THEN stmt_part { $$ = actions->condition($1, $3); }
  ;

loop_stmt: LOOP do_cond ENDLOOP { $$ = actions->loop($2); }
  ;

do_cond: expr DO stmt_part { $$ = actions->condition($1, $3); }
  ;

empty_stmt: SKIP { $$ = actions->empty_stmt(); }
  ;

block_stmt: block { $$ = actions->block_stmt($1); }
  ;

proc_stmt: CALL name proc_args { $$ = actions->proc_stmt($2, $3); }
  ;

proc_args: LHRND var_access_list RHRND { $$ = $2; }
  | /* epsilon */ { $$ = std::vector<std::shared_ptr<Expr>>(); }
  ;

/* should take var access list at most. this will allow read 1 + 2; */
read_stmt: READ var_access_list { $$ = actions->io($2, symbol::READ); }
  | READLN var_access { $$ = actions->readline($2); }
  ;


/* Expressions */
expr_list: expr_list COMMA expr { $1.push_back($3); $$ = $1; }
  | expr { $$ = std::vector<std::shared_ptr<Expr>>{$1}; }
  | error { $$ = std::vector<std::shared_ptr<Expr>>(); yyerrok; }
  ;

expr: expr prim_op prime_expr { $$ = actions->binary($2, $1, $3); }
  | prime_expr { $$ = $1; }
  ;

prime_expr: prime_expr rel_op simple_expr { $$ = actions->binary($2, $1, $3); }
  | simple_expr { $$ = $1; }
  ;

simple_expr: simple_expr add_op term { $$ = actions->binary($2, $1, $3); }
  | term { $$ = $1; }
  ;

term: factor mult_op factor { $$ = actions->binary($2, $1, $3); }
  | factor { $$ = $1; }
  ;

factor: number { $$ = $1; }
  | character  { $$ = $1; }
  | bool_sym  { $$ = $1; }
  | string { $$ = $1; }
  | endl { $$ = $1; }
  | var_access  { $$ = $1; }
  | LHRND expr RHRND { $$ = $2; }
  | NOT factor { $$ = actions->unary(symbol::NOT, $2); }
  | MINUS factor { $$ = actions->unary(symbol::MINUS, $2); }
  ;


/* Variables */
var_list: var_list COMMA name { $1.push_back($3); $$ = $1; }
  | name { $$ = std::vector<std::string>{$1}; }
  ;

var_access_list: var_access_list COMMA var_access { $1.push_back($3); $$ = $1; }
  | var_access { $$ = std::vector<std::shared_ptr<Expr>>{$1}; }
  ;

var_access: name selector { $$ = actions->access($1, $2); }
  | var_access DOT name selector { $$ = actions->rec_access($1, $3, $4); }
  ;

selector: LHSQR expr RHSQR { $$ = $2; }
  | /* epsilon */
    { auto expr = actions->empty_expr(); expr->set_null(true); $$ = expr; }
  ;


/* Operators Terminals */
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


/* Value Based Terminals */
type_sym: INT { $$ = actions->new_type(symbol::INT); }
  | FLOAT { $$ = actions->new_type(symbol::FLOAT); }
  | BOOL { $$ = actions->new_type(symbol::BOOL); }
  | CHAR { $$ = actions->new_type(symbol::CHAR); }
  ;

constant: number { $$ = $1; }
  | bool_sym  { $$ = $1; }
  | character { $$ = $1; }
  | name { $$ = actions->access($1, actions->empty_expr()); }
  | string { $$ = $1; }
  | endl { $$ = $1; }
  ;

character: CHARACTER { $$ = actions->constant(symbol::CHAR, temp_ch); }
  ;

number: num DOT NUMBER { $$ = actions->constant(symbol::FLOAT, $1, yytext); }
  | num { $$ = actions->constant(symbol::INT, $1); }
  ;

num: NUMBER { $$ = temp_num; }
  ;

bool_sym: TRUE { $$ = actions->constant(symbol::TRUE, 1); }
  | FALSE { $$ = actions->constant(symbol::FALSE, 0); }
  ;

string: STRING { $$ = actions->const_string(std::string(yytext)); }
  ;

endl: NEWLINE { $$ = actions->constant(symbol::CHAR, '\n'); }
  ;

name: NAME { $$ = std::string(yytext); }
  ;
%%

