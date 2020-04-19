%{
#include <string>
#include <iostream>
#include <vector>
#include "Actions.h"
#include "AstNode.h"
#include "Symbol.h"
#include "Types.h"
#include "parser.tab.hh"
#define YY_DECL extern "C" int yylex()

extern Actions* actions;
%}

%option nounput noyywrap

letter [a-zA-Z]                                                                
digit [0-9]                                                                    
number {digit}*
name ({letter}|_)({letter}|digit|_)*
character :.


%%
begin     { return BEG; }
end       { return END; }

"."       { return DOT; }
","       { return COMMA; }
";"       { return SEMI; }
"("       { return LHRND; }
")"       { return RHRND; }
"["       { return LHSQR; }
"]"       { return RHSQR; }

"+"       { return PLUS; }
"-"       { return MINUS; }
"*"       { return MULT; }
"/"       { return DIV; }
"%"       { return MOD; }

and       { return AND; }
or        { return OR; }
not       { return NOT; }

"=="        { return EQ; }
"~="        { return NEQ; }
"<"         { return LESS; }
">"         { return GREATER; }
"<="        { return LEQ; }
">="        { return GEQ; }

write     { return WRITE;}
if        { return IF; }
do        { return DO; }
"end if"  { return ENDIF; }
"else if" { return ELIF; }
while     { return LOOP; }
"end while" { return ENDLOOP; }
"="       { return INIT; }
":="        { return ASGN; }
skip      { return SKIP; }
call      { return CALL; }
read      { return READ; }

const     { return CONST; }
int       { return INT; }
bool      { return BOOL; }
float     { return FLOAT; }
char      { return CHAR; }

array     { return ARRAY; }
proc      { return PROC; }
"end proc" { return ENDPROC; }
record    { return RECORD; }
"end record" { return ENDREC; }
type      { return TYPE; }

true      { return TRUE; }
false     { return FALSE; }
{name}    { return NAME; }
{number}  { return atoi(yytext); }
'.'        { return yytext[1]; }

\n        { actions->get_admin()->newline(); }
[ \t]     { /*Ignore Whitespace*/ }
<<EOF>>   { return 0; }

--[^\n]* { /* ignore rest of line for comments */ }
.       { printf("Invalid Char: %c\n", *yytext); }
%%
