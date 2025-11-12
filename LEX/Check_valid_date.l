DATE 0[1-9]|[1-2][0-9]|3[0-1]
MONTH 0[1-9]|1[012]
YEAR [0-9]{4}
%{
    // PROGRAM TO DETECT VALID DATE
    #include <stdio.h>
    #include <stdlib.h>

    int isLeapYear(int y) {
        if (y % 400 == 0) return 1;
        else if (y % 4 == 0 && y % 100 != 0) return 1;
        return 0;
    }
%}
%%
{DATE}"/"{MONTH}"/"{YEAR} {
    int d, m, y;
    if (sscanf(yytext, "%2d/%2d/%4d", &d, &m, &y) != 3) {
        printf("Invalid Date (parse error): %s\n", yytext); 
        YY_BREAK;
    }

    if (y == 0) {
        printf("Invalid Date (year cant be 0): %s\n", yytext); 
        YY_BREAK;
    }
    int maxDay = 30;
    if (m == 2) {
        maxDay = isLeapYear(y)? 29 : 28;
    }
    else if (m == 1 || m == 3 || m == 5 || m ==7 || m == 8 || m == 10 || m == 12) maxDay = 31;
    
    if (d >= 1 && d <= maxDay)    printf("Valid Date %s\n", yytext);
    else  printf("Invalid Date (day %d out of range for month %02d, year %04d): %s\n", d, m, y, yytext);
}
[ \t\r\n]* {;}
[0-9/]+       { printf("Invalid Input. Not a valid date: %s\n", yytext); }
[a-zA-Z_][a-zA-Z_0-9]*  { printf("Invalid Input. Not a valid date: %s\n", yytext); }
.  { printf("Invalid Input. Not a valid date: %s\n", yytext); }
%%

int main() {
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}