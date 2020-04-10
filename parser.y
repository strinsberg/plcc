%{
#include <stdio.h>

extern int line;
extern char* yytext;
int yylex();
int yyerror(char*);
%}


%token BEG END
%token COMMA DOT SEMI
%token LHRND RHRND LHSQR RHSQR
%token WRITE ASGN IF DO ELIF ENDIF LOOP ENDLOOP SKIP
%token AND OR NOT
%token INIT EQ NEQ LESS GREATER LEQ GEQ
%token PLUS MINUS MULT DIV MOD
%token ARRAY PROC ENDPROC RECORD ENDREC TYPE
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
  | /* epsilon */
  ;

def: const_def { printf("def -> const_def\n\n"); }
  | var_def { printf("def -> var_def\n\n"); }
  ;

const_def: CONST type_sym name ASGN constant { printf("const_def\n"); }
  ;

var_def: type_sym v_prime { printf("var_def\n"); }
  ;

v_prime: var_list { printf("v_prime -> var_list\n"); }
  | ARRAY LHSQR constant RHSQR var_list { printf("v_prime -> array\n"); }
  ;



/* Statements */
stmt_part: stmt_part stmt SEMI { printf("stmt_part\n"); }
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

asn_stmt: var_access_list ASGN expr_list { printf("assignment_stmt\n"); }
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

/* Expressions */
expr_list: expr_list COMMA expr { printf("expr_list\n"); }
  | expr
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

factor: number { printf("factor -> number\n"); }
  | char { printf("factor -> char\n"); }
  | bool_sym { printf("factor -> bool_sym\n"); }
  | var_access { printf("factor -> var_access\n"); }
  | LHRND expr RHRND { printf("factor -> ( expr )\n"); }
  | NOT factor { printf("factor -> NOT\n"); }
  ;


/* Variables */
var_list: var_list COMMA name { printf("var_list\n"); }
  | name
  ;

var_access_list: var_access_list COMMA var_access { printf("var_access_list\n"); }
  | var_access
  ;

var_access: name selector { printf("var_access\n"); }
  ;

selector: LHSQR constant RHSQR { printf("selector -> array access\n"); }
  | /* epsilon */
  ;


/* Operators */
prim_op: AND { printf("AND\n"); }
  | OR { printf("OR\n"); }
  ;

rel_op: EQ { printf("EQUAL\n"); }
  | NEQ { printf("NOT EQUAL\n"); }
  | LESS { printf("LESS\n"); }
  | GREATER { printf("GREATER\n"); } 
  | LEQ { printf("LESS EQUAL\n"); }
  | GEQ { printf("GREATER EQUAL\n"); }
  ;

add_op: PLUS { printf("PLUS\n"); }
  | MINUS { printf("MINUS\n"); }
  ;

mult_op: MULT { printf("MULT\n"); }
  | DIV { printf("DIV\n"); }
  | MOD { printf("MOD\n"); }
  ;


/* Terminals */
type_sym: INT { printf("INT\n"); }
  | FLOAT { printf("FLOAT\n"); }
  | BOOL { printf("BOOL\n"); }
  | CHAR { printf("CHAR\n"); }
  ;

constant: number { printf("constant -> number\n"); }
  | bool_sym { printf("constant -> bool\n"); } 
  | name { printf("constant -> name\n"); }
  | char { printf("constant -> char\n"); }
  ;

char: CHARACTER { printf("CHARACTER -> '%c'\n", $1); }
  ;

number: NUMBER DOT NUMBER {printf("number -> float: %d.%d\n", $1, $3); }
  | NUMBER { printf("number -> int: %d\n", $1); }
  ;

bool_sym: TRUE { printf("FALSE\n"); }
  | FALSE { printf("TRUE\n"); }
  ;

name: NAME { printf("NAME -> %s\n", yytext); }
  ;
%%


int main() {
  yyparse();
}


int yyerror(char *s) {
  fprintf(stderr, "error: %s, %s\n", s, yytext);
}
