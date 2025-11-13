#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

struct Productions {
    char lhs;
    char rhs[20];
} prod[MAX];

int prodCnt = 0;
char nonTerminals[MAX];
int nonTermCnt = 0;

char terminals[MAX];
int termCnt = 0;

char firstSets[26][MAX];
char followSets[26][MAX];
int visitedFirst[26];
int visitedFollow[26];

void addSymbol(char *set, char c) {
    // c not in set
    if (strchr(set, c) == NULL) {
        int len = strlen(set);
        set[len] = c;
        set[len + 1] = '\0';
    } 
}

int isNonTerminal(char c) {
    return (c >= 'A' && c <= 'Z');
}

void splitProductions(char *input) {
    char lhs = input[0];
    char *rhs = strtok(input + 2, "|"); // skip A=
    while (rhs != NULL) {
        prod[prodCnt].lhs = lhs;
        strcpy(prod[prodCnt].rhs, rhs);
        prodCnt++;
        rhs = strtok(NULL, "|");
    }
}

void computeFirst(char symbol, char* result) {
    // 1. add self
    if (!isNonTerminal(symbol)) {
        addSymbol(result, symbol);
        return;
    }

    // 2. check cache
    int index = symbol - 'A';
    if (visitedFirst[index]) {
        strcpy(result, firstSets[index]);
        return;
    }
    visitedFirst[index]=1;

    // for all prods - add first
    for (int i = 0; i < prodCnt; i++) {
        if (prod[i].lhs == symbol) {
            // null transitions
            if (prod[i].rhs[0] == '#') {
                addSymbol(result, '#');
            }
            else {
                int j = 0;
                // ε is #
                // Eg: S->ABC
                // FIRST(A)={a,#}
                // FIRST(B)={b}

                // get first(A) and store in temp
                // add all symbols of temp to result
                // if temp has #, then we have to add first(B), else we can stop
                // If we reach the end of RHS (means every symbol so far produced ε),
                // -> then the entire production can produce ε.
                while (prod[i].rhs[j] != '\0') {
                    char temp[MAX] = "";
                    computeFirst(prod[i].rhs[j], temp);
                    for (int t = 0; temp[t] != '\0'; t++) {
                        if (temp[t] != '#') addSymbol(result, temp[t]);
                    }
                    // is # not in temp -- we can stop no need to check further
                    if (strchr(temp, '#') == NULL) break;
                    j++;
                    // only reached if every prodn so far reached #
                    if (prod[i].rhs[j] == '\0') addSymbol(result, '#');
                }
            }
        }
    }
    strcpy(firstSets[index], result);
}
// E= T R
// R= + T R | #
// T= F Y
// Y= * F Y | #
// F= ( E ) | i

void computeFollow(char symbol, char* result) {
    int index = symbol - 'A';
    // 1. check cache
    if (visitedFollow[index]) {
        strcpy(result, followSets[index]);
        return;
    }
    visitedFollow[index] = 1;

    // 2. X is start symbol add $
    if (symbol == prod[0].lhs) addSymbol(result, '$');

    // 3. check in each production
    for (int i = 0; i < prodCnt; i++) {
        // if symbol not in prod.rhs skip prod
        if (strchr(prod[i].rhs, symbol) == NULL) continue;

        int j = 0;
        // 4.check in rhs
        while (prod[i].rhs[j] != '\0') {
            // locate our symbol
            if (prod[i].rhs[j] == symbol) {
                // R#1: X is at end - add followset of lhs
                if (prod[i].rhs[j + 1] == '\0') {
                    // R#1.1: if X is at end and LHS is also X - skip it
                    if (prod[i].lhs == symbol) {
                        j++;
                        continue;
                    } 

                    char temp[MAX] = "";
                    computeFollow(prod[i].lhs, temp);
                    for (int t = 0; t < strlen(temp); t++) addSymbol(result, temp[t]);
                }
                // R#2: X is followed by terminal next - add terminal
                else if (!isNonTerminal(prod[i].rhs[j + 1])) {
                    addSymbol(result, prod[i].rhs[j + 1]);
                }
                // R#3: X is followed by nonterminal NEXT - add first(next)- {ε}
                else {
                    int nextSymbolHasNULLInFirstSet = 0;
                    
                    // temp = FIRST(next) – {ε}
                    char temp[MAX] = "";
                    computeFirst(prod[i].rhs[j + 1], temp);
                    for (int t = 0; t < strlen(temp); t++) {
                        if (temp[t] != '#') addSymbol(result, temp[t]);
                        else nextSymbolHasNULLInFirstSet = 1;
                    }

                    // R#3.1: X is followed by NEXT that ⇒ ε -> add follow of lhs
                    if (nextSymbolHasNULLInFirstSet) {
                        char temp[MAX] = "";
                        if (prod[i].lhs == symbol) {
                            j++;
                            continue;
                        } 
                        computeFollow(prod[i].lhs, temp);
                        for (int t = 0; t < strlen(temp); t++) addSymbol(result, temp[t]);
                    }
                }      
            }
            j++;
        }
    }
    strcpy(followSets[index], result);
}

int main() {
    char input[100];
    printf("Enter productions (use # for epsilon, | for multiple RHS).\n");
    printf("Enter rules (empty line to stop):\n");
    while (1) {
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        if (input[0] == '\n') break;
        input[strcspn(input, "\n")] = 0; // strip newline

        if (strlen(input) > 0)
            splitProductions(input);
    }

    // Collect non terminals
    for (int i = 0; i < prodCnt; i++) {
        if (strchr(nonTerminals, prod[i].lhs) == NULL) nonTerminals[nonTermCnt++] = prod[i].lhs;
    }

    // FIRST Sets
    for (int i = 0; i < nonTermCnt; i++) {
        char result[MAX] = "";
        computeFirst(nonTerminals[i], result);
    }

    printf("FIRST SETS: \n");
    for (int i =0 ; i < nonTermCnt; i++) {
        printf("FIRST(%c) = { ", nonTerminals[i]);
        for (int j = 0; j < strlen(firstSets[nonTerminals[i]-'A']); j++) {
            if (j > 0) printf(" ");
            printf("%c", firstSets[nonTerminals[i]-'A'][j]);
        }
        printf("}\n");
    }

    // FOLLOW Sets
    for (int i = 0; i < nonTermCnt; i++) {
        char result[MAX] = "";
        computeFollow(nonTerminals[i], result);
    }

    printf("FOLLOW SETS: \n");
    for (int i =0 ; i < nonTermCnt; i++) {
        printf("FOLLOW(%c) = { ", nonTerminals[i]);
        for (int j = 0; j < strlen(followSets[nonTerminals[i]-'A']); j++) {
            if (j > 0) printf(" ");
            printf("%c", followSets[nonTerminals[i]-'A'][j]);
        }
        printf("}\n");
    }
    return 0;
}