// This program:
// 1. Builds LL(1) parsing table for given grammar
// 2. Uses a stack to parse an input string
// 3. Accepts/rejects the string

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX 100

// Parsing table for the grammar
// Non-terminals: E, E', T, T', F
// Terminals: i, +, *, (, ), $
char table[5][6][10] = {
    /* E  */ {"TE'", "", "", "TE'", "", ""},
    /* E' */ {"", "+TE'", "", "", "#", "#"},
    /* T  */ {"FT'", "", "", "FT'", "", ""},
    /* T' */ {"", "#", "*FT'", "", "#", "#"},
    /* F  */ {"i", "", "", "(E)", "", ""}
};

char *nonTerm[] = {"E", "E'", "T", "T'", "F"};   // Non-terminals
char term[] = {'i', '+', '*', '(', ')', '$'};    // Terminals

// Function to get row for non-terminal
int row(char *sym) {
    for (int i = 0; i < 5; i++)
        if (strcmp(nonTerm[i], sym) == 0)
            return i;
    return -1;
}

// Function to get column for terminal
int column(char c) {
    for (int i = 0; i < 6; i++)
        if (term[i] == c)
            return i;
    return -1;
}

// Function to check if a symbol is a non-terminal (E, E', T, T', F)
int isNonTerminal(char *sym) {
    for (int i = 0; i < 5; i++) {
        if (strcmp(nonTerm[i], sym) == 0) return 1;
    }
    return 0;
}

int main() {
    char stack[MAX][MAX];  // stack of symbols (each can be E, E', T etc.)
    char input[MAX];
    int top = 0;

    printf("Enter input string (end with $): ");
    scanf("%s", input);

    // initialize stack
    strcpy(stack[top], "$");
    strcpy(stack[++top], "E"); // start symbol

    char *p = input;
    printf("\nStack\t\tInput\t\tAction\n");

    while (top >= 0) {
        // print stack
        for (int i = 0; i <= top; i++) printf("%s", stack[i]);
        printf("\t\t%s\t\t", p);

        // if top of stack is terminal
        if (strlen(stack[top]) == 1 && stack[top][0] == *p) {
            printf("Match %c\n", *p);
            top--;
            p++;
        }
        else if (isNonTerminal(stack[top])) {
            int r = row(stack[top]);
            int c = column(*p);
            char *prod = table[r][c];
            if (strlen(prod) == 0) {
                printf("Error\n");
                break;
            }
            else if (strcmp(prod, "#") == 0) { // epsilon
                printf("%s -> ε\n", stack[top]);
                top--;
            }
            else {
                printf("%s -> %s\n", stack[top], prod);
                char tmp[MAX];
                strcpy(tmp, prod);
                top--;

                // push RHS in reverse order (each symbol could be E', T', etc.)
                for (int j = strlen(tmp) - 1; j >= 0;) {
                    if (j > 0 && tmp[j] == '\'') { // handle E', T'
                        char sym[3] = {tmp[j-1], '\'', '\0'};
                        strcpy(stack[++top], sym);
                        j -= 2;
                    } else {
                        char sym[2] = {tmp[j], '\0'};
                        strcpy(stack[++top], sym);
                        j--;
                    }
                }
            }
        }
        else {
            printf("Error\n");
            break;
        }

        if (strcmp(stack[top], "$") == 0 && *p == '$') {
            printf("Accept\n");
            break;
        }
    }
    return 0;
}
