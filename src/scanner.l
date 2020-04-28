%{
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include "Actions.h"
#include "AstNode.h"
#include "Symbol.h"
#include "Types.h"
#include "parser.tab.hh"

// From parser.tab.h for admin::error() and admin::newline()
extern std::shared_ptr<Actions> actions;
extern FILE* yyin;
%}

%option nounput noyywrap noinput batch

letter [a-zA-Z]                                                                
digit [0-9]                                                                    
number {digit}*
name ({letter}|_)({letter}|digit|_)*
character :.
string \"[^\"]*\"

/* might be possible to use bison for single chars to reduce rules here */
%%
begin         { return yy::parser::token::BEG; }
end           { return yy::parser::token::END; }

"."           { return yy::parser::token::DOT; }
","           { return yy::parser::token::COMMA; }
";"           { return yy::parser::token::SEMI; }
"("           { return yy::parser::token::LHRND; }
")"           { return yy::parser::token::RHRND; }
"["           { return yy::parser::token::LHSQR; }
"]"           { return yy::parser::token::RHSQR; }

"+"           { return yy::parser::token::PLUS; }
"-"           { return yy::parser::token::MINUS; }
"*"           { return yy::parser::token::MULT; }
"/"           { return yy::parser::token::DIV; }
"%"           { return yy::parser::token::MOD; }

and           { return yy::parser::token::AND; }
or            { return yy::parser::token::OR; }
not           { return yy::parser::token::NOT; }

"=="          { return yy::parser::token::EQ; }
"~="          { return yy::parser::token::NEQ; }
"<"           { return yy::parser::token::LESS; }
">"           { return yy::parser::token::GREATER; }
"<="          { return yy::parser::token::LEQ; }
">="          { return yy::parser::token::GEQ; }

write         { return yy::parser::token::WRITE;}
if            { return yy::parser::token::IF; }
then          { return yy::parser::token::THEN; }
do            { return yy::parser::token::DO; }
"end if"      { return yy::parser::token::ENDIF; }
"else if"     { return yy::parser::token::ELIF; }
while         { return yy::parser::token::LOOP; }
"end while"   { return yy::parser::token::ENDLOOP; }
"="           { return yy::parser::token::INIT; }
":="          { return yy::parser::token::ASGN; }
skip          { return yy::parser::token::SKIP; }
call          { return yy::parser::token::CALL; }
read          { return yy::parser::token::READ; }
readln        { return yy::parser::token::READLN; }

const         { return yy::parser::token::CONST; }
int           { return yy::parser::token::INT; }
bool          { return yy::parser::token::BOOL; }
float         { return yy::parser::token::FLOAT; }
char          { return yy::parser::token::CHAR; }

array         { return yy::parser::token::ARRAY; }
proc          { return yy::parser::token::PROC; }
"end proc"    { return yy::parser::token::ENDPROC; }
record        { return yy::parser::token::RECORD; }
"end record"  { return yy::parser::token::ENDREC; }
type          { return yy::parser::token::TYPE; }

true          { return yy::parser::token::TRUE; }
false         { return yy::parser::token::FALSE; }
endl          { return yy::parser::token::NEWLINE; }

{name}        { return yy::parser::token::NAME; }
{number}      { return yy::parser::token::NUMBER; }
'.'           { return yy::parser::token::CHARACTER; }
{string}      { return yy::parser::token::STRING; }

\n            { actions->get_admin()->newline(); }
[ \t]         { /*Ignore Whitespace*/ }
<<EOF>>       { yyterminate(); return 0; }

--[^\n]*      { /* ignore rest of line for comments */ }
.             { actions->get_admin()->error("bad char '" + std::string(yytext) + "'"); }
%%
