// TAC to Basic Blocks
// R#1: First instruction is always a leader
// R#2: Find all labels that are targets of goto/if statements
// R#3: Instruction following a goto/if is a leader

// sample input:
// a = b + c
// if a < 10 goto L1
// d = a * 2
// L1:
// e = d + 1

// output:
// Block 1:
// a = b + c
// if a < 10 goto L1
// Block 2:
// d = a * 2
// Block 3:
// L1:
// e = d + 1

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#define MAX 256

char targetLabels[MAX][MAX];
int tIn = 0;

typedef struct {
    char instruction[MAX];
    int isLeader;
} Instruction;

void trim(char* string) {
    int n = (int)strlen(string);
    int start = 0;
    while (start < n && isspace(string[start])) start++;
    int end = n - 1;
    while (end >= start && isspace(string[end])) end--;
    int len = 0;
    for (int i = start; i <= end; i++) {
        string[len++] = string[i];
    }
    string[len] = '\0';
}

int hasLabel(char* instruction) {
    char* end = strchr(instruction, ':');
    return (end != NULL)? 1: 0;
}
int hasGoTo(char* instruction) {
    char* start = strstr(instruction, "goto");
    return (start != NULL)? 1: 0;
}
void getLabel(char* instruction, char* label) {
    char* end = strchr(instruction, ':');
    if (end) {
        int len = end - instruction;
        strncpy(label, instruction,len);
        label[len] = '\0';
    }
}

void getTargetLabel(char* instruction, char* label) {
    char* start = strstr(instruction, "goto") + 4;
    if (start) {;
        int i = 0;
        while (i< MAX - 1 && isspace(*start)) {start++; i++;}
        
        i = 0;
        while (i < MAX - 1 && !isspace(*start)) label[i++] = *start++;
        label[i] = '\0';
        trim(label);
    }
}

int isTargetLabel(char* label) {
    for (int i = 0; i < tIn; i++) {
        if (strcmp(label, targetLabels[i]) == 0) return 1;
    }
    return 0;
}
void addToTargetLabels(char* label) {
    if (label == NULL || label[0] == '\0') return;
    if (!isTargetLabel(label) && tIn < MAX) {
        strncpy(targetLabels[tIn],label, MAX-1);
        targetLabels[tIn][MAX-1] = '\0';
        tIn++;
    }
}

int main() {
    Instruction instructions[MAX];
    int instructionsCnt = 0;
    char line[MAX];
    printf("Enter TAC: \n");
    while (fgets(line, sizeof(line), stdin)) {
        trim(line);
        if (line[0] == '\0') continue;      
        strncpy(instructions[instructionsCnt].instruction, line, MAX-1);
        instructions[instructionsCnt].instruction[MAX-1] = '\0';
        instructions[instructionsCnt].isLeader = 0;
        instructionsCnt++;
    }

    // first instruction is a leader
    instructions[0].isLeader = 1;
    int prevWasGoTo = 0;
    for (int i = 0; i < instructionsCnt; i++) {
        if (hasGoTo(instructions[i].instruction)) {
            char label[MAX];
            getTargetLabel(instructions[i].instruction, label);
            addToTargetLabels(label);
        }
    }

    for (int i = 0; i < tIn; i++) {
        printf("%s", targetLabels[i]);
    }

    for (int i = 1; i < instructionsCnt; i++) {
        if (hasLabel(instructions[i].instruction)){
            char label[MAX];
            getLabel(instructions[i].instruction, label);
            if (isTargetLabel(label)) {
                instructions[i].isLeader = 1;
            }
        }
        if (prevWasGoTo) {
            instructions[i].isLeader = 1;
            prevWasGoTo = 0;
        }
        if (hasGoTo(instructions[i].instruction)) {
            prevWasGoTo = 1;
        }
    }
    printf("\nBasic Blocks:");
    int blockCnt = 0;
    for (int i = 0; i < instructionsCnt; i++) {
        if (instructions[i].isLeader) {printf("\nBlock %d: \n", blockCnt + 1); blockCnt++;}
        printf("%s\n", instructions[i].instruction);
    }
}