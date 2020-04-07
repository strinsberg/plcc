%{
#include <stdio.h>

int yylex();
int yyerror(char*);
%}


%token BEG END
%token COMMA DOT SEMI
%token LHRND RHRND
%token WRITE
%token PLUS MINUS MULT DIV MOD
%token NUMBER TRUE FALSE


%%
program:  /* nothing */
  | block DOT { printf("program\n"); }
  ;

block: BEG def_part stmt_part END { printf("block\n"); }
  ;

def_part: /* zero occurences */ { printf("def_part -> epsilon\n"); }
  | def SEMI def_part { printf("def_part -> def ; def_part\n"); }
  ;

def: /* do nothing for now */ { printf("def\n"); }
  ;

stmt_part: /* zero occurences */ { printf("stmt_part -> epsilon\n"); }
  | stmt SEMI stmt_part { printf("stmt_part -> stmt ; stmt_part\n"); }
  ;

stmt: write_stmt { printf("stmt\n"); }
  ;

write_stmt: WRITE expr_list { printf("write_stmt\n"); }
  ;

expr_list: expr { printf("expr_list -> expr\n"); }
  | expr COMMA expr_list { printf("expr_list -> expr , expr_list\n"); }
  ;

expr: simple_expr { printf("expr\n"); }
  ;

simple_expr: term { printf("simple_expr -> term\n"); }
  | term add_op simple_expr { printf("simple_expr -> term add_op simple_expr\n"); }
  ;

add_op: PLUS { printf("PLUS\n"); }
  | MINUS { printf("MINUS\n"); }
  ;

term: factor { printf("term -> factor\n"); }
  | factor mult_op term { printf("term -> factor mult_op term\n"); }
  ;

mult_op: MULT { printf("MULT\n"); }
  | DIV { printf("DIV\n"); }
  | MOD { printf("MOD\n"); }
  ;

factor: constant { printf("factor -> constant\n"); }
  | LHRND expr RHRND { printf("factor -> ( expr )\n"); }
  ;

constant: NUMBER { printf("NUMBER = %d\n", $1); }
  | TRUE { printf("FALSE\n"); }
  | FALSE { printf("TRUE\n"); }
  ;

%%


int main() {
  yyparse();
}


int yyerror(char *s) {
  fprintf(stderr, "error: %s\n", s);
}
