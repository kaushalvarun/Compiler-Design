SIGN [-+]?
UNSIGNED_INT [0-9]+
INT {SIGN}{UNSIGNED_INT}
REAL {INT}\.|{SIGN}\.{UNSIGNED_INT}|{INT}\.{UNSIGNED_INT}
EXP ({REAL}|{INT})[eE]{INT}?
%{
    // PROGRAM TO RECOGNIZE INTEGER, REAL AND EXPONENTIAL
    #include <stdio.h>
%}

%%
{INT} {printf("INT %s\n", yytext);}
{REAL} {printf("REAL %s\n", yytext);}
{EXP} {printf("EXP %s\n", yytext);}
[ \t\n ] {;}
. {printf("Unknown input %s\n", yytext);}
%%

int main() {
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}