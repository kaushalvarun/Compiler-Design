// 1. Write a C program to convert a given switch-case statement into Three-Address
// Code (TAC) using labels and conditional/unconditional jumps.
// switch(a) {
// case 1: x = 10; break;
// case 2: x = 20; break;
// default: x = 0;
// }
// output:
// if a == 1 goto L1
// if a == 2 goto L2
// goto L3
// L1: x = 10
// goto L4
// L2: x = 20
// goto L4
// L3: x = 0
// L4:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#define MAX 256

typedef struct {
    int value;
    int defaultCase;
    char action[MAX];
} Case;

void trim(char* string) {
    int len = strlen(string);
    int i = 0;
    int j = len - 1;
    while (i < len && isspace(string[i])) i++;
    while (j >= i && isspace(string[j])) j--;
    
    int l = 0;
    for (int k = i; k <= j; k++) {
        string[l++] = string[k];
    }
    string[l] = '\0';
}

void getSwitchVar(char* line, char* switchVar) {
    char variable[MAX];
    char* start = strchr(line, '(') + 1;
    char* end = strchr(line, ')');
    int len = end - start;
    strncpy(variable, start, len);
    variable[len]='\0';
    trim(variable);
    strcpy(switchVar, variable);
}

void getCaseValueAndAction(char* line, Case* c, int defaultCase) {
    char caseOrDefault[100];
    strcpy(caseOrDefault, defaultCase? "default" : "case");
    int offset = (int)strlen(caseOrDefault);
    char* start = strstr(line, caseOrDefault) + offset;
    char* end = strchr(line, ':');
    char val[MAX];
    if (start && end) {
        int len = end - start;
        strncpy(val, start, len);
        val[len]='\0';
        trim(val);
    }
    else {
        printf("Error getting value\n");
        strcpy(val,"-1");
    }

    
    char* actStart = end + 1;
    char* actEnd = strstr(line, "break;");
    char action[MAX];
    if (!actEnd) actEnd = strstr(line, ";");
    int actlen = actEnd - actStart;
    if (actStart && actEnd) {
        strncpy(action, actStart, actlen);
        trim(action);
    }
    if (action[strlen(action)-1] == ';') action[strlen(action)-1] = '\0';
    c->value = atoi(val);
    strcpy(c->action, action);
    c->defaultCase = defaultCase;
}

int main() {
    printf("Please provide switch case statement: (enter ctrl D when done):\n");
    char line[MAX];
    int inSwitch = 0;
    Case cases[MAX];
    int numCases = 0;
    char switchVar[MAX];

    while (fgets(line, sizeof(line), stdin)) {
        trim(line);
        // check for switch
        if (strstr(line, "switch")) {
            // get switch var
            getSwitchVar(line, switchVar);
            inSwitch = 1;
            continue;
        }

        // get case
        if (inSwitch) {
            if (strstr(line, "case")) {
                getCaseValueAndAction(line, &cases[numCases++],0);
                continue;
            }

            else if (strstr(line, "default")) {
                getCaseValueAndAction(line, &cases[numCases++],1);
                continue;
            }

            else if (strchr(line, '}')) break;
        }
    }

    printf("=====TAC CODE======\n");
    // generate TAC or show error
    if (inSwitch) {
        // print the if statements
        for(int c = 0; c < numCases-1; c++) {
            printf("if %s = %d goto L%d\n", switchVar, cases[c].value, c+1);
        }

        // handle default case
        if (cases[numCases - 1].defaultCase) {
            printf("goto L%d\n", numCases);
        }
        else {
            printf("if %s = %d goto L%d\n", switchVar, cases[numCases-1].value, numCases);
        }

        // handle the L labels
        for(int c = 0; c < numCases; c++) {
            printf("L%d: %s\ngoto L%d\n", c+1, cases[c].action, numCases + 1);
        }
        printf("L%d: \n", numCases + 1);
    }
    else {
        printf("Error: No Switch present!\n");
    }
    return 0;
}