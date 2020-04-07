%{
#include "parser.tab.h"

int line = 0;
%}


letter [a-zA-Z]                                                                
digit [0-9]                                                                    
number {digit}*
name ({letter}|_)({letter}|digit|_)*


%%
begin { return BEG; }
end   { return END; }

"."   { return DOT; }
","   { return COMMA; }
";"   { return SEMI; }
"("   { return LHRND; }
")"   { return RHRND; }

"+"   { return PLUS; }
"-"   { return MINUS; }
"*"   { return MULT; }
"/"   { return DIV; }
"%"   { return MOD; }

write { return WRITE;}

true  { return TRUE; }
false { return FALSE; }
{number} { yylval = atoi(yytext); return NUMBER; }

\n      { line++; }
[ \t]   { /*Ignore Whitespace*/ }
<<EOF>> { return 0; }

.       { printf("Invalid Char: %c\n", *yytext); }
%%
