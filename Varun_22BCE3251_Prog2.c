// 2. Write a C program to convert a given sequence of Three-Address Code (TAC)
// instructions into Basic Blocks. Your program should:
// Identify the leaders in the TAC (first instruction, targets of goto/if, and
// instructions following a goto/if).
// Divide the TAC into Basic Blocks based on the identified leaders.
// Print each Basic Block with a clear label (e.g., Block 1, Block 2, …).
// Input: A sequence of TAC instructions.
// Output: Basic Blocks with their constituent instructions.
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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_INSTRUCTIONS 100
#define MAX_LINE 200

typedef struct {
    char instruction[MAX_LINE];
    int isLeader;
} Instruction;

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

// Check if instruction is a goto statement
int isGoto(char *str) {
    return strstr(str, "goto") != NULL;
}

// Check if instruction is a conditional jump (if statement)
int isConditionalJump(char *str) {
    return strstr(str, "if") != NULL && strstr(str, "goto") != NULL;
}

// Check if instruction is a label
int isLabel(char *str) {
    int len = strlen(str);
    if (len > 0 && str[len-1] == ':') {
        return 1;
    }
    return 0;
}

// Extract label from goto/if statement
void extractLabel(char *str, char *label) {
    char *gotoPos = strstr(str, "goto");
    if (gotoPos) {
        gotoPos += 4; // Skip "goto"
        while (*gotoPos && isspace(*gotoPos)) gotoPos++;
        
        int i = 0;
        while (*gotoPos && !isspace(*gotoPos)) {
            label[i++] = *gotoPos++;
        }
        label[i] = '\0';
    }
}

// Check if a label matches
int labelMatches(char *instruction, char *label) {
    char instrLabel[50];
    int len = strlen(instruction);
    
    if (len > 0 && instruction[len-1] == ':') {
        strncpy(instrLabel, instruction, len-1);
        instrLabel[len-1] = '\0';
        return strcmp(instrLabel, label) == 0;
    }
    return 0;
}

// Identify leaders in TAC
void identifyLeaders(Instruction instructions[], int count) {
    int i;
    char labels[MAX_INSTRUCTIONS][50];
    int labelCount = 0;
    
    // First instruction is always a leader
    if (count > 0) {
        instructions[0].isLeader = 1;
    }
    
    // Find all labels that are targets of goto/if statements
    for (i = 0; i < count; i++) {
        if (isGoto(instructions[i].instruction) || isConditionalJump(instructions[i].instruction)) {
            extractLabel(instructions[i].instruction, labels[labelCount]);
            labelCount++;
            
            // Instruction following a goto/if is a leader
            if (i + 1 < count) {
                instructions[i + 1].isLeader = 1;
            }
        }
    }
    
    // Mark instructions with labels as leaders
    for (i = 0; i < count; i++) {
        if (isLabel(instructions[i].instruction)) {
            instructions[i].isLeader = 1;
        } else {
            // Check if this instruction's label is a target
            for (int j = 0; j < labelCount; j++) {
                if (labelMatches(instructions[i].instruction, labels[j])) {
                    instructions[i].isLeader = 1;
                }
            }
        }
    }
}

// Print Basic Blocks
void printBasicBlocks(Instruction instructions[], int count) {
    int blockNum = 1;
    int i;
    
    printf("\n=== Basic Blocks ===\n\n");
    
    for (i = 0; i < count; i++) {
        if (instructions[i].isLeader) {
            if (i > 0) {
                printf("\n");
            }
            printf("Block %d:\n", blockNum++);
        }
        printf("%s\n", instructions[i].instruction);
    }
}

int main() {
    Instruction instructions[MAX_INSTRUCTIONS];
    char line[MAX_LINE];
    int count = 0;
    
    printf("Enter TAC instructions (press Ctrl+D or Ctrl+Z when done):\n");
    
    // Read instructions from input
    while (fgets(line, sizeof(line), stdin)) {
        trim(line);
        
        // Skip empty lines
        if (strlen(line) == 0) {
            continue;
        }
        
        strcpy(instructions[count].instruction, line);
        instructions[count].isLeader = 0;
        count++;
    }
    
    if (count == 0) {
        printf("\nNo instructions entered!\n");
        return 1;
    }
    
    // Identify leaders
    identifyLeaders(instructions, count);
    
    // Print basic blocks
    printBasicBlocks(instructions, count);
    
    return 0;
}