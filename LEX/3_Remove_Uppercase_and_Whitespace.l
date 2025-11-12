%{
    //3. PROGRAM TO REMOVE UPPERCASE AND WHITESPACE
    #include <stdio.h>
%}

%%
[A-Z]   ;
[ \t]   ;
\n      {ECHO;}
.       {printf("%s", yytext);}
%%

int main() {
    fprintf(stderr, "Enter input text (Ctrl+D to finish):\n");
    yylex();
    return 0;
}

int yywrap() {
    return 1;
}