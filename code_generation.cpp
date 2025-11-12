#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <sstream>

using namespace std;

// Token structure
struct Token {
    string type;  // "NUM", "OP", "LPAREN", "RPAREN"
    string value;
};

// AST Node structure
struct ASTNode {
    string type;  // "BINOP", "VAR"
    string op;    // operator for BINOP
    string value; // variable name or number
    ASTNode* left;
    ASTNode* right;
    
    ASTNode() : left(NULL), right(NULL) {}
};

class Lexer {
public:
    vector<Token> tokenize(const string& expr) {
        vector<Token> tokens;
        for (size_t i = 0; i < expr.length(); i++) {
            char c = expr[i];
            
            // Skip whitespace
            if (isspace(c)) continue;
            
            // Identifiers and numbers
            if (isalnum(c)) {
                string val;
                while (i < expr.length() && isalnum(expr[i])) {
                    val += expr[i++];
                }
                i--;
                Token t;
                t.type = "VAR";
                t.value = val;
                tokens.push_back(t);
            }
            // Operators
            else if (c == '+' || c == '-' || c == '*' || c == '/') {
                Token t;
                t.type = "OP";
                t.value = string(1, c);
                tokens.push_back(t);
            }
            // Parentheses
            else if (c == '(') {
                Token t;
                t.type = "LPAREN";
                t.value = "(";
                tokens.push_back(t);
            }
            else if (c == ')') {
                Token t;
                t.type = "RPAREN";
                t.value = ")";
                tokens.push_back(t);
            }
        }
        return tokens;
    }
};

class Parser {
private:
    vector<Token> tokens;
    size_t pos;
    
    ASTNode* parseExpression();
    ASTNode* parseTerm();
    ASTNode* parseFactor();
    
public:
    ASTNode* parse(const vector<Token>& t) {
        tokens = t;
        pos = 0;
        return parseExpression();
    }
};

ASTNode* Parser::parseExpression() {
    ASTNode* node = parseTerm();
    
    while (pos < tokens.size() && tokens[pos].type == "OP" && 
           (tokens[pos].value == "+" || tokens[pos].value == "-")) {
        string op = tokens[pos].value;
        pos++;
        ASTNode* right = parseTerm();
        
        ASTNode* binop = new ASTNode();
        binop->type = "BINOP";
        binop->op = op;
        binop->left = node;
        binop->right = right;
        node = binop;
    }
    
    return node;
}

ASTNode* Parser::parseTerm() {
    ASTNode* node = parseFactor();
    
    while (pos < tokens.size() && tokens[pos].type == "OP" && 
           (tokens[pos].value == "*" || tokens[pos].value == "/")) {
        string op = tokens[pos].value;
        pos++;
        ASTNode* right = parseFactor();
        
        ASTNode* binop = new ASTNode();
        binop->type = "BINOP";
        binop->op = op;
        binop->left = node;
        binop->right = right;
        node = binop;
    }
    
    return node;
}

ASTNode* Parser::parseFactor() {
    if (tokens[pos].type == "LPAREN") {
        pos++;
        ASTNode* node = parseExpression();
        pos++; // skip RPAREN
        return node;
    }
    
    ASTNode* node = new ASTNode();
    node->type = "VAR";
    node->value = tokens[pos].value;
    pos++;
    return node;
}

class CodeGenerator {
private:
    int regCount;
    vector<string> instructions;
    
public:
    CodeGenerator() : regCount(0) {}
    
    int generate(ASTNode* node) {
        if (node == NULL) return -1;
        
        if (node->type == "VAR") {
            // Load variable into register
            int reg = regCount++;
            stringstream ss;
            ss << "MOV R" << reg << ", " << node->value;
            instructions.push_back(ss.str());
            return reg;
        }
        
        else if (node->type == "BINOP") {
            // Generate code for left subtree
            int leftReg = generate(node->left);
            
            // Generate code for right subtree
            int rightReg = generate(node->right);
            
            // Perform operation
            string op;
            if (node->op == "+") op = "ADD";
            else if (node->op == "-") op = "SUB";
            else if (node->op == "*") op = "MUL";
            else op = "DIV";
            
            stringstream ss;
            ss << op << " R" << leftReg << ", R" << rightReg;
            instructions.push_back(ss.str());
            
            return leftReg;
        }
        
        return -1;
    }
    
    void printInstructions() {
        for (size_t i = 0; i < instructions.size(); i++) {
            cout << instructions[i] << endl;
        }
    }
};

int main() {
    string expression;
    cout << "Enter an arithmetic expression: ";
    cin >> expression;
    cout << "\nGenerated Assembly Code:" << endl;
    cout << "=======================" << endl << endl;
    
    // Lexical Analysis
    Lexer lexer;
    vector<Token> tokens = lexer.tokenize(expression);
    
    // Syntax Analysis
    Parser parser;
    ASTNode* ast = parser.parse(tokens);
    
    // Code Generation
    CodeGenerator codegen;
    codegen.generate(ast);
    codegen.printInstructions();
    
    return 0;
}