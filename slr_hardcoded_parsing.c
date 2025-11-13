#include <stdio.h>
#include <string.h>

#define MAX 200

typedef struct {
    char action[10];
} AG;
    
AG actionTable[12][6] = {
    {{"s5"}, {""}, {""}, {"s4"}, {""}, {""}},
    {{""}, {"s6"}, {""}, {""}, {""}, {"accept"}},
    {{""}, {"r2"}, {"s7"}, {""}, {"r2"}, {"r2"}},
    {{""}, {"r4"}, {"r4"}, {""}, {"r4"}, {"r4"}},
    {{"s5"}, {""}, {""}, {"s4"}, {""}, {""}},
    {{""}, {"r6"}, {"r6"}, {""}, {"r6"}, {"r6"}},
    {{"s5"}, {""}, {""}, {"s4"}, {""}, {""}},
    {{"s5"}, {""}, {""}, {"s4"}, {""}, {""}},
    {{""}, {"s6"}, {""}, {""}, {"s11"}, {""}},
    {{""}, {"r1"}, {"s7"}, {""}, {"r1"}, {"r1"}},
    {{""}, {"r3"}, {"r3"}, {""}, {"r3"}, {"r3"}},
    {{""}, {"r5"}, {"r5"}, {""}, {"r5"}, {"r5"}}
};

int gotoTable[12][3] = {
    {1, 2, 3}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
    {8, 2, 3}, {0, 0, 0}, {0, 9, 3}, {0, 0, 10},
    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
};

char *productions[] = {
    "", "E->E+T", "E->T", "T->T*F", "T->F", "F->(E)", "F->id"
};

int termIndex(const char *sym) {
    if (strcmp(sym, "id") == 0) return 0;
    if (strcmp(sym, "+") == 0) return 1;
    if (strcmp(sym, "*") == 0) return 2;
    if (strcmp(sym, "(") == 0) return 3;
    if (strcmp(sym, ")") == 0) return 4;
    if (strcmp(sym, "$") == 0) return 5;
    return -1;
}

int nontermIndex(char A) {
    if (A == 'E') return 0;
    if (A == 'T') return 1;
    if (A == 'F') return 2;
    return -1;
}

typedef struct {
    int state;
    char symbol[10];
} StackEntry;

StackEntry stack[MAX];
int top = -1;

void push(int s, const char *sym) {
    top++;
    stack[top].state = s;
    strcpy(stack[top].symbol, sym);
}

void popN(int n) {
    top -= n;
}

int main() {
    char input[500];
    printf("Enter input string ending with $");
    if (scanf("%s", input) != 1) return 0;

    char tokens[200][10];
    int tcount = 0;
    for (int i = 0; i < (int)strlen(input);) {
        if (input[i] == 'i' && input[i + 1] == 'd') {
            strcpy(tokens[tcount++], "id");
            i += 2;
        } else {
            char tmp[2] = {input[i], 0};
            strcpy(tokens[tcount++], tmp);
            i++;
        }
    }

    int ip = 0;
    push(0, "$");

    while (1) {
        int state = stack[top].state;
        int col = termIndex(tokens[ip]);
        if (col == -1) {
            printf("Lexical error\n");
            break;
        }

        char *action = actionTable[state][col].action;

        if (action[0] == 's') {
            int next = action[1] - '0';
            push(next, tokens[ip]);
            ip++;
            printf("Shift %s, push state %d\n", tokens[ip - 1], next);
        } else if (action[0] == 'r') {
            int prod = action[1] - '0';
            char *rhs = strchr(productions[prod], '>') + 1;
            int rhslen = (strcmp(rhs, "id") == 0) ? 1 : (int)strlen(rhs);
            if (strcmp(rhs, "ε") == 0) rhslen = 0;
            popN(rhslen);
            int prevState = stack[top].state;
            char lhs[2] = {productions[prod][0], 0};
            int gcol = nontermIndex(lhs[0]);
            int next = gotoTable[prevState][gcol];
            push(next, lhs);
            printf("Reduce using %s, goto %d\n", productions[prod], next);
        } else if (strcmp(action, "accept") == 0) {
            printf("Input accepted\n");
            break;
        } else {
            printf("Error\n");
            break;
        }
    }

    return 0;
}
