SIGN [+-]?
%{
    // PROGRAM TO DETECT ODD/ EVEN 
    #include <stdio.h>
%}

%%
{SIGN}[0-9]*[02468]$ {printf("EVEN %s\n", yytext);}
{SIGN}[0-9]*[13579]$ {printf("ODD %s\n", yytext);}
[ \t\r\n]* {;}
[a-zA-Z_][a-zA-Z_0-9]* {printf("Invalid Input. Please Enter a number. \n");}
. {printf("Invalid Input. Please Enter a number. \n");}
%%

int main() {
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}