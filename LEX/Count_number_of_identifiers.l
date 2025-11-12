digit [0-9]
letter [a-zA-Z]
%{
    #include <stdio.h>
    int identifier_cnt = 0;
%}

%%
{letter}({digit}|{letter})* identifier_cnt++;
%%

int main() {
    yylex();
    printf("Number of identifiers:  %d\n",identifier_cnt);
    return 0;
}

int yywrap() {
    return 1;
}