%{
    #include <stdio.h>
%}

%%
(https?:"//")?([wW]{3}\.)?[a-zA-Z_0-9]+\.[a-zA-Z_0-9]+(("/"[a-zA-Z_0-9]*)|(:[0-9]+))? {printf("Valid URL!: %s\n", yytext);}
[ \r\t\n] {;}
[a-zA-Z_0-9]+ {printf("Invalid URL: %s\n", yytext);}
. {printf("Invalid URL: %s\n", yytext);}
%%

int main() {
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}

