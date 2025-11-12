%{
    // Lex program to count the frequency of the given word in a file
    #include <stdio.h>
    #include <strings.h>
    #include <string.h>
    static long freq = 0;
    char word[256];
    extern FILE *yyin;
%}

%%
[a-zA-Z0-9_]+ {
    if(strcasecmp(word, yytext) == 0) freq++;
}
[ \r\n\t] {;}
. {;}
%%

int main(int argC, char** argv) {
    if (argC < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    printf("Enter word: ");
    scanf("%255s", word);
    FILE* file;
    file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Error: Can't open %s\n", argv[1]);
        return 1;
    }
    yyin= file;
    yylex();
    printf("%s occured %ld times int %s\n",word, freq, argv[1]);
    fclose(file);
    return 0;
}

int yywrap() {
    return 1;
}