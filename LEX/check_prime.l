%{
/* Lex program to check whether a number is Prime or Not */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

%}

%option noyywrap

%%
-?[0-9]+ {
    long long num = strtoll(yytext, NULL, 10);

    if (num < 2) {
        printf("%lld is Not prime!\n", num);
        continue;
    }

    int isPrime = 1;
    for (long long i = 2; i <= num / i; ++i) {
        if (num % i == 0) { printf("%lld is Not prime!\n", num); isPrime = 0; break; }
    }
    if (isPrime) printf("%lld is prime!\n", num);
}
[0-9]*[A-Za-z_]+[0-9]*   { printf("Invalid number token: %s\n", yytext); }
[ \r\n\t]+               { /* skip whitespace */ }
.                        { /* ignore other chars */ }
%%

int main(void) {
    yylex();
    return 0;
}
