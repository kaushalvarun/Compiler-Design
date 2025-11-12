%{
    // LEX code to extract HTML tags from a file
    #include <stdio.h>
    extern FILE *yyin;
%}

%s SCRIPT
%%
"<"[ \t]*[sS][cC][rR][iI][pP][tT][^>]*">" {
    printf("TAG: %s\n", yytext);
    BEGIN(SCRIPT);
}           
<SCRIPT>"</"[ \t]*[sS][cC][rR][iI][pP][tT][^>]*">" {
    printf("TAG: %s\n", yytext);
    BEGIN(INITIAL);
}
<SCRIPT>"<"[^<]+">"   {;}
<SCRIPT>[^<]+   {;}

"<"[ \t]*"/"?[a-zA-Z][^>]*">" {

    printf("TAG: %s\n",yytext);
}
"<!--"[ \t*][^>]*"-->" {;}
"</"[a-zA-Z !]+">" {;}
[ \r\n\t] {;}
. {;}
%%

int main(int argC, char** argv) {
    if (argC < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE* file;
    file = fopen(argv[1], "r");
    if (!file) {
        fprintf(stderr, "Error: Can't open %s\n", argv[1]);
        return 1;
    }
    yyin= file;
    yylex();
    fclose(file);
    return 0;
}

int yywrap() {
    return 1;
}