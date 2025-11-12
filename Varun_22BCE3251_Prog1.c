// 1. Write a C program to convert a given switch-case statement into Three-Address
// Code (TAC) using labels and conditional/unconditional jumps.
// Requirements:
// The program should have the switch variable, case values, actions, and default
// action defined in the code itself.
// Generate TAC using the following rules:
// Each case should have a conditional jump to its label.
// The default case should have a jump if no cases match.
// Each case should end with an unconditional jump to the exit label.
// Print the resulting Three-Address Code clearly with labels.
// sample input /output:
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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_CASES 50
#define MAX_LINE 200

typedef struct {
    int value;
    char action[100];
} Case;

// Function to trim whitespace
void trim(char *str) {
    int i, j = 0;
    int len = strlen(str);
    int start = 0, end = len - 1;
    
    while (start < len && isspace(str[start])) start++;
    while (end >= start && isspace(str[end])) end--;
    
    for (i = start; i <= end; i++) {
        str[j++] = str[i];
    }
    str[j] = '\0';
}

// Function to extract switch variable
void extractSwitchVar(char *line, char *var) {
    char *start = strchr(line, '(');
    char *end = strchr(line, ')');
    if (start && end) {
        int len = end - start - 1;
        strncpy(var, start + 1, len);
        var[len] = '\0';
        trim(var);
    }
}

// Function to extract case value
int extractCaseValue(char *line) {
    char *start = strstr(line, "case");
    if (start) {
        start += 4;
        while (*start && isspace(*start)) start++;
        return atoi(start);
    }
    return 0;
}

// Function to extract action from case
void extractAction(char *line, char *action) {
    char *start = strchr(line, ':');
    char *end = strstr(line, "break");
    
    if (!end) end = strchr(line, ';');
    
    if (start && end) {
        start++;
        int len = end - start;
        strncpy(action, start, len);
        action[len] = '\0';
        trim(action);
        
        // Remove trailing semicolon if present
        len = strlen(action);
        if (len > 0 && action[len-1] == ';') {
            action[len-1] = '\0';
        }
    }
}

// Function to generate TAC
void generateTAC(char *switchVar, Case cases[], int numCases, char *defaultAction) {
    int i;
    
    printf("\n=== Three-Address Code ===\n\n");
    
    // Generate conditional jumps for each case
    for (i = 0; i < numCases; i++) {
        printf("if %s == %d goto L%d\n", switchVar, cases[i].value, i + 1);
    }
    
    // Jump to default case
    printf("goto L%d\n", numCases + 1);
    
    // Generate labels and actions for each case
    for (i = 0; i < numCases; i++) {
        printf("L%d: %s\n", i + 1, cases[i].action);
        printf("goto L%d\n", numCases + 2);
    }
    
    // Generate default case
    printf("L%d: %s\n", numCases + 1, defaultAction);
    
    // Exit label
    printf("L%d:\n", numCases + 2);
}

int main() {
    char line[MAX_LINE];
    char switchVar[50] = "";
    Case cases[MAX_CASES];
    int numCases = 0;
    char defaultAction[100] = "";
    int inSwitch = 0;
    
    printf("Enter the switch-case statement (press Ctrl+D or Ctrl+Z when done):\n");
    
    while (fgets(line, sizeof(line), stdin)) {
        trim(line);
        
        // Check for switch statement
        if (strstr(line, "switch")) {
            extractSwitchVar(line, switchVar);
            inSwitch = 1;
            continue;
        }
        
        // Check for case statement
        if (strstr(line, "case") && inSwitch) {
            cases[numCases].value = extractCaseValue(line);
            extractAction(line, cases[numCases].action);
            numCases++;
            continue;
        }
        
        // Check for default statement
        if (strstr(line, "default") && inSwitch) {
            char *start = strchr(line, ':');
            if (start) {
                start++;
                strcpy(defaultAction, start);
                trim(defaultAction);
                // Remove trailing semicolon
                int len = strlen(defaultAction);
                if (len > 0 && defaultAction[len-1] == ';') {
                    defaultAction[len-1] = '\0';
                }
            }
            continue;
        }
        
        // Check for closing brace
        if (strchr(line, '}') && inSwitch) {
            break;
        }
    }
    
    // Generate TAC
    if (strlen(switchVar) > 0 && numCases > 0) {
        generateTAC(switchVar, cases, numCases, defaultAction);
    } else {
        printf("\nError: Invalid switch-case statement format!\n");
    }
    
    return 0;
}