#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

class Token {
public:
    string type; // NUM, OP, LPAREN, RPAREN
    string value;
    Token(string t, string v) {
        type = t;
        value = v;
    }
};

// Abstract Syntax Tree
class ASTNode {
public:
    string type; // "BINOP", VAR
    string op; // operator for BINOP
    string val; // variable name or number
    ASTNode* left; 
    ASTNode* right;

    ASTNode() {
        left= nullptr;
        right = nullptr;
    }
};

// Tokenize the expression
class Lexer {
public:
    vector<Token> tokenize(string& expression) {
        vector<Token> tokens;
        for (int i = 0; i < expression.size(); i++) {
            char c = expression[i];

            // Skip whitespace
            if (isspace(c)) continue;

            // Identifiers and numbers
            if (isalnum(c) || c == '_') {
                string val;
                while (i < expression.size() && (isalnum(expression[i]) || expression[i] == '_')) {
                    val += expression[i];
                    i++;
                }
                i--;
                Token t("VAR", val);
                tokens.push_back(t);
            }

            // Operators
            else if (c == '+' || c == '-' || c == '*' || c == '/') {
                Token t("OP", string(1,c));
                tokens.push_back(t);
            }

            // Parentheses
            else if (c == '(') {
                Token t("LPAREN", "(");
                tokens.push_back(t);
            }

             else if (c == ')') {
                Token t("RPAREN", ")");
                tokens.push_back(t);
            }
        }
        return tokens;
    }
};

// Build Syntax Tree
 class Parser {
private:
    vector<Token> tokens;
    int pos;

    // GRAMMAR
    // Enforces precedence and left assosciativity
    // Expression->Term {(+|-) Term}*
    // Term->Factor {(*|/) Factor}*
    // Factor->VAR | '('Expression')'

    ASTNode* parseExpression() {
        ASTNode* node = parseTerm();
        while (pos < tokens.size() &&  tokens[pos].type=="OP" && 
               (tokens[pos].value == "+" || tokens[pos].value == "-")) {
            string op = tokens[pos].value;
            pos++;
            ASTNode* right = parseTerm();
            ASTNode* binop = new ASTNode();
            binop->op=op;
            binop->type="BINOP";
            binop->left= node;
            binop->right= right;
            node = binop;
        }
        return node;
    }

    ASTNode* parseTerm() {
        ASTNode* node = parseFactor();
        while (pos < tokens.size() &&  tokens[pos].type=="OP" && 
               (tokens[pos].value == "*" || tokens[pos].value == "/")) {
            string op = tokens[pos].value;
            pos++;
            ASTNode* right = parseFactor();
            ASTNode* binop = new ASTNode();
            binop->op=op;
            binop->type="BINOP";
            binop->left= node;
            binop->right= right;
            node = binop;
        }
        return node;
    }

    ASTNode* parseFactor() {
        if (tokens[pos].type == "LPAREN") {
            pos++;
            ASTNode* node = parseExpression();
            pos++; // skip "RPAREN"
            return node;
        }
        ASTNode* node = new ASTNode();
        node->type = "VAR";
        node->val = tokens[pos].value;
        pos++;
        return node;
    }

public:
    ASTNode* parse(const vector<Token>& t) {
        tokens = t;
        pos = 0;
        return parseExpression();
    }
};

class CodeGenerator {
private:
    int regCount;
    vector<string> instructions;
public:
    CodeGenerator() {
        regCount = 0;
    }

    int generate(ASTNode* node) {
        if (node == nullptr) return -1;
        if (node->type == "VAR") {
            int reg = regCount;
            regCount++;
            stringstream ss;
            ss << "MOV R" << reg << ", " << node->val;
            instructions.push_back(ss.str());
            return reg;
        }
        else if (node->type == "BINOP") {
            // Generate Left sub tree code
            int leftReg = generate(node->left);
            // Generate right sub tree code
            int rightReg = generate(node->right);

            // Perform operation
            string op;
            if (node->op == "+") op = "ADD";
            else if (node->op == "-") op = "SUB";
            else if (node->op == "*") op = "MUL";
            else op = "DIV";

            stringstream ss;
            ss << op  << " R"<< leftReg << ", R" << rightReg;
            instructions.push_back(ss.str());
            return leftReg;
        }

        return -1;
    }
    void printInstructions() {
        for (int i = 0; i < instructions.size(); i++) {
            cout << instructions[i] << "\n";
        }
    }
};

int main() {
    string expression;
    cout << "Enter an arithmetic expression: ";
    cin >> expression;
    
    // Lexer
    Lexer lexer;
    vector<Token> tokens = lexer.tokenize(expression);
    cout << "Tokens: " << "\n";
    cout << "====================================" << "\n\n";
    for (int i = 0; i < tokens.size(); i++) {
        cout << "{" << tokens[i].type << "," << tokens[i].value << "}\n";
    }
    cout << "\n";
    // Build Syntax Tree
    Parser parser;
    ASTNode* ast = parser.parse(tokens);

    // Generate Assembly Code
    cout << "\nGenerated Assembly Code: " << "\n";
    cout << "====================================" << "\n\n";
    CodeGenerator codegen;
    codegen.generate(ast);
    codegen.printInstructions();
    return 0;
}

