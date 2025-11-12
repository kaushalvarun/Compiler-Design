#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cstdlib>

using namespace std;

struct Statement {
    string lhs;           // left-hand side variable
    string rhs;           // right-hand side expression
    string op;            // operator (+, -, *, /)
    string operand1;      // first operand
    string operand2;      // second operand
    bool isConstant;      // whether both operands are constants
    bool isAssignment;    // whether it's a simple assignment
};

class Optimizer {
private:
    vector<Statement> statements;
    map<string, int> constantValues;
    map<string, bool> isConstantVar;
    
    bool isNumber(const string& str) {
        if (str.empty()) return false;
        for (size_t i = 0; i < str.length(); i++) {
            if (!isdigit(str[i])) return false;
        }
        return true;
    }
    
    int evaluateExpression(const string& op1, const string& op, const string& op2) {
        int val1 = atoi(op1.c_str());
        int val2 = atoi(op2.c_str());
        
        if (op == "+") return val1 + val2;
        else if (op == "-") return val1 - val2;
        else if (op == "*") return val1 * val2;
        else if (op == "/") return (val2 != 0) ? val1 / val2 : 0;
        
        return 0;
    }
    
    void parseStatement(const string& line, Statement& stmt) {
        size_t eqPos = line.find('=');
        stmt.lhs = line.substr(0, eqPos);
        
        // Remove spaces
        stmt.lhs.erase(0, stmt.lhs.find_first_not_of(" \t"));
        stmt.lhs.erase(stmt.lhs.find_last_not_of(" \t") + 1);
        
        string rhs = line.substr(eqPos + 1);
        rhs.erase(0, rhs.find_first_not_of(" \t;"));
        rhs.erase(rhs.find_last_not_of(" \t;") + 1);
        
        stmt.rhs = rhs;
        stmt.isAssignment = true;
        stmt.isConstant = false;
        
        // Check if it's a binary operation
        size_t opPos = string::npos;
        string foundOp = "";
        
        for (size_t i = 1; i < rhs.length(); i++) {
            char c = rhs[i];
            if (c == '+' || c == '-' || c == '*' || c == '/') {
                opPos = i;
                foundOp = string(1, c);
                break;
            }
        }
        
        if (opPos != string::npos) {
            stmt.operand1 = rhs.substr(0, opPos);
            stmt.operand2 = rhs.substr(opPos + 1);
            stmt.op = foundOp;
            
            // Remove spaces
            stmt.operand1.erase(stmt.operand1.find_last_not_of(" \t") + 1);
            stmt.operand2.erase(0, stmt.operand2.find_first_not_of(" \t"));
            
            stmt.isConstant = isNumber(stmt.operand1) && isNumber(stmt.operand2);
        } else {
            stmt.operand1 = rhs;
            stmt.op = "";
            stmt.operand2 = "";
        }
    }
    
public:
    void readInput() {
        string line;
        while (getline(cin, line)) {
            // Remove leading/trailing whitespace
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line == "END" || line.empty()) break;
            
            Statement stmt;
            parseStatement(line, stmt);
            statements.push_back(stmt);
        }
    }
    
    // Constant Folding: Evaluate constant expressions at compile time
    void constantFolding() {
        for (size_t i = 0; i < statements.size(); i++) {
            if (statements[i].isConstant && statements[i].op != "") {
                int result = evaluateExpression(statements[i].operand1, 
                                               statements[i].op, 
                                               statements[i].operand2);
                statements[i].operand1 = to_string(result);
                statements[i].op = "";
                statements[i].operand2 = "";
                statements[i].isConstant = false;
                
                // Store constant value
                constantValues[statements[i].lhs] = result;
                isConstantVar[statements[i].lhs] = true;
            }
        }
    }
    
    // Constant Propagation: Replace variables with their constant values
    void constantPropagation() {
        for (size_t i = 0; i < statements.size(); i++) {
            // Propagate operand1
            if (constantValues.find(statements[i].operand1) != constantValues.end()) {
                statements[i].operand1 = to_string(constantValues[statements[i].operand1]);
            }
            
            // Propagate operand2
            if (constantValues.find(statements[i].operand2) != constantValues.end()) {
                statements[i].operand2 = to_string(constantValues[statements[i].operand2]);
            }
            
            // Check if now we can do constant folding
            if (statements[i].op != "" && 
                isNumber(statements[i].operand1) && isNumber(statements[i].operand2)) {
                int result = evaluateExpression(statements[i].operand1, 
                                               statements[i].op, 
                                               statements[i].operand2);
                statements[i].operand1 = to_string(result);
                statements[i].op = "";
                statements[i].operand2 = "";
                
                constantValues[statements[i].lhs] = result;
                isConstantVar[statements[i].lhs] = true;
            }
        }
    }
    
    // Algebraic Simplification: Simplify expressions like x*1, x*0, x+0
    void algebraicSimplification() {
        for (size_t i = 0; i < statements.size(); i++) {
            if (statements[i].op == "") continue;
            
            string op1 = statements[i].operand1;
            string op = statements[i].op;
            string op2 = statements[i].operand2;
            
            // x + 0 = x
            if (op == "+" && op2 == "0") {
                statements[i].operand1 = op1;
                statements[i].op = "";
                statements[i].operand2 = "";
            }
            // 0 + x = x
            else if (op == "+" && op1 == "0") {
                statements[i].operand1 = op2;
                statements[i].op = "";
                statements[i].operand2 = "";
            }
            // x - 0 = x
            else if (op == "-" && op2 == "0") {
                statements[i].operand1 = op1;
                statements[i].op = "";
                statements[i].operand2 = "";
            }
            // x * 1 = x
            else if (op == "*" && op2 == "1") {
                statements[i].operand1 = op1;
                statements[i].op = "";
                statements[i].operand2 = "";
            }
            // 1 * x = x
            else if (op == "*" && op1 == "1") {
                statements[i].operand1 = op2;
                statements[i].op = "";
                statements[i].operand2 = "";
            }
            // x * 0 = 0
            else if (op == "*" && op2 == "0") {
                statements[i].operand1 = "0";
                statements[i].op = "";
                statements[i].operand2 = "";
            }
            // 0 * x = 0
            else if (op == "*" && op1 == "0") {
                statements[i].operand1 = "0";
                statements[i].op = "";
                statements[i].operand2 = "";
            }
            // x / 1 = x
            else if (op == "/" && op2 == "1") {
                statements[i].operand1 = op1;
                statements[i].op = "";
                statements[i].operand2 = "";
            }
        }
    }
    
    // Dead Code Elimination: Remove statements where x = x
    void deadCodeElimination() {
        vector<Statement> optimized;
        
        for (size_t i = 0; i < statements.size(); i++) {
            // x = x is dead code, remove it
            if (statements[i].op == "" && 
                statements[i].lhs == statements[i].operand1) {
                continue;
            }
            optimized.push_back(statements[i]);
        }
        
        statements = optimized;
    }
    
    void optimize() {
        cout << "\nApplying optimizations..." << endl;
        cout << "1. Constant Folding" << endl;
        constantFolding();
        
        cout << "2. Constant Propagation" << endl;
        constantPropagation();
        
        cout << "3. Algebraic Simplification" << endl;
        algebraicSimplification();
        
        cout << "4. Dead Code Elimination" << endl;
        deadCodeElimination();
    }
    
    void printOptimized() {
        cout << "\nOptimized Code:" << endl;
        cout << "===============" << endl;
        
        for (size_t i = 0; i < statements.size(); i++) {
            cout << statements[i].lhs << " = ";
            
            if (statements[i].op == "") {
                cout << statements[i].operand1;
            } else {
                cout << statements[i].operand1 << " " << statements[i].op 
                     << " " << statements[i].operand2;
            }
            cout << ";" << endl;
        }
    }
};

int main() {
    cout << "Compiler Optimization Techniques" << endl;
    cout << "================================" << endl;
    cout << "Enter code (type END to finish):" << endl;
    
    Optimizer opt;
    opt.readInput();
    
    cout << "\nOriginal Code:" << endl;
    cout << "==============" << endl;
    
    opt.optimize();
    opt.printOptimized();
    
    return 0;
}