// Simple Lexical Analyzer - C code to identify tokens
// RECOGNISES TOKENS
// =====================================
// identifiers (Sum, _x1, var123)
// integer literals (3, 42)
// assignment operator =
// arithmetic operators + - * /
// semicolon ; (end of statement)
// parentheses ( ) (optional)

// Example run (input: Sum=3 + 2;):
// Lexeme          : Token Category
// ---------------------------------------
// Sum             : "identifier"
// =               : "assignment operator"
// 3               : "integer literal"
// +               : "addition operator"
// 2               : "integer literal"
// ;               : "end of the statement"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define MAX 256
void emit(const char* lexeme, const char* category) {
    printf("%-15s | %-15s\n", lexeme, category);
}

void tokenize(char* input) {
    int i = 0;
    int n = (int)strlen(input);
    while (i < n) {
        // Skip whitespace
        while (i < n && isspace(input[i])) {i++;}

        // identifier: [A-Za-z_][A-Za-z0-9_]
        if (i < n && isalpha(input[i]) || input[i] == '_') {
            char buffer[MAX];
            int buffLen = 0;
            if (buffLen < MAX-1) buffer[buffLen++] = input[i++];
            while (i < n && isalnum(input[i])) {
                buffer[buffLen++] = input[i];
                i++;
            }
            buffer[buffLen] = '\0';
            emit(buffer, "\"identifier\"");
            continue;
        }

        // integer literal: [0-9]+ (simple)
        if (i < n && isnumber(input[i])) {
            char buffer[MAX];
            int buffLen = 0;
            buffer[buffLen++] = input[i];
            i++;
            while (i < n && isnumber(input[i])) {
                buffer[buffLen++] = input[i];
                i++;
            }
            buffer[buffLen] = '\0';
            emit(buffer, "\"integer literal\"");
            continue;
        }

        // single-character tokens
        switch (input[i]) {
            case '=':
                emit("=", "\"assignment operator\"");
                i++;
                break;
            case '+':
                emit("+", "\"addition operator\"");
                i++;
                break;
            case '-':
                emit("-", "\"subtraction operator\"");
                i++;
                break;
            case '*':
                emit("*", "\"multiplication operator\"");
                i++;
                break;
            case '/':
                emit("/", "\"division operator\"");
                i++;
                break;
            case ';':
                emit(";", "\"end of the statement\"");
                i++;
                break;
            case '(':
                emit("(", "\"left paren\"");
                i++;
                break;
            case ')':
                emit(")", "\"right paren\"");
                i++;
                break;
            default: {
                /* unknown / unhandled single character */
                char unknown[4] = { input[i], '\0', '\0', '\0' };
                emit(unknown, "\"unknown\"");
                i++;
                break;
            }
        }
    }
}


int main(void) {
    char input[1024];
    printf("Enter a C expression (single line). Press Enter then Ctrl+D (or Ctrl+Z on Windows) to end input:\n");
    /* read whole stdin into input buffer (simple) */
    if (!fgets(input, sizeof(input), stdin)) {
        return 0;
    }

    /* If user wants to allow spaces and more lines, you can loop fgets; here single-line is enough */
    emit("Lexeme", "category");
    for (int i = 0; i < 31; i++) printf("-");
    printf("\n");
    
    tokenize(input);

    return 0;
}