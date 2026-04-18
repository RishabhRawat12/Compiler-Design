#ifndef TAC_H
#define TAC_H

#include <string>
#include <vector>
#include <sstream>

using namespace std;

// Representing a single Three-Address Code instruction
struct TACInstruction {
    string op;     // Operator (e.g., +, -, IF, GOTO)
    string arg1;   // Operand 1
    string arg2;   // Operand 2
    string result; // Result or label

    TACInstruction(string o, string a1, string a2, string r)
        : op(o), arg1(a1), arg2(a2), result(r) {}

    string toString() const {
        if (op == "=") return result + " = " + arg1;
        if (op == "+" || op == "-" || op == "*" || op == "/") 
            return result + " = " + arg1 + " " + op + " " + arg2;
        if (op == "LABEL") return result + ":";
        if (op == "GOTO") return "goto " + result;
        if (op == "IF") return "if " + arg1 + " " + arg2 + " goto " + result;
        if (op == "PARAM") return "param " + arg1;
        if (op == "CALL") return result + " = call " + arg1 + ", " + arg2;
        if (op == "RETURN") return "return " + arg1;
        if (op == "BEGIN_FUNC") return "header " + result;
        if (op == "END_FUNC") return "end function";
        return result + " = " + arg1 + " " + op + " " + arg2;
    }
};

#endif
