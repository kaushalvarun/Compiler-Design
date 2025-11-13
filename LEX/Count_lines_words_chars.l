%{
// scanner that counts the number of characters, words,
// and lines in a file
#include <stdio.h>
int nchars = 0;
int nwords = 0;
int nlines = 0;
%}

%%
\n {nlines++; nchars++;}
[^ \t\n]+ {nwords++; nchars += yyleng;}
. {nchars++;}
%%

int main(int argC, char* argv[]) {
    FILE *inpFile;

    if (argC < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    inpFile = fopen(argv[1], "r");
    if (!inpFile) {
        fprintf(stderr, "Error: Could not open file %s\n", argv[0]);
        return 1;
    }

    yyin = inpFile;
    yylex();
    printf("Number of chars: %d\n", nchars);
    printf("Number of words: %d\n", nwords);
    printf("Number of lines: %d\n", nlines);
    fclose(inpFile);
    return 0;
}

int yywrap() {
    return 1;
}
