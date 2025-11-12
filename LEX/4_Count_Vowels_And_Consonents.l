%{
    // Section-1: C declarations (included in generated C file)
    // Section-2: Rules Section: patterns and actions
    // User Subroutines (optional)

    #include <stdio.h>
    int vowels = 0, consonants = 0;
%}

%%

[\n\t]+ ;
[aeiouAEIOU] {vowels++; }
[^aeiouAEIOU] {consonants++; }
.             ;
%%

int main() {
    printf("Enter an input string: ");
    yylex();
    printf("Vowel Count: %d\n", vowels);
    printf("Consonants Count: %d\n", consonants);
}


int yywrap() {
    return 1;
}