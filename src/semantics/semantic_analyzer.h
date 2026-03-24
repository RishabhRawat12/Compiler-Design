#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <fstream>
#include "../../symbol_table.h" // Root se uthaya
#include "../AST/stmt.h"        // AST folder se
#include "../AST/expr.h"        // AST folder se

using namespace std;

// 1. DataType Enum: Types ko classify karne ke liye
enum DataType {
    TYPE_INT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_UNKNOWN
};

// 2. Error Structure: Line wise track rakhne ke liye
struct SemanticError {
    string message;
    int line;
};

// 3. Main Analyzer Class
class SemanticAnalyzer {
private:
    SymbolTable& symTable;           // Existing table ka reference
    vector<SemanticError> errors;    // Saare errors yahan store honge
    set<string> reportedErrors;      // Duplicate reporting se bachne ke liye
    map<string, bool> isUsed;        // Kaunsa variable use hua?
    map<string, bool> isInitialized; // Kaunsa variable define hua?

    // --- Private Handlers (The "Brain" of the analyzer) ---
    
    // AST Traversal functions
    void visitStmt(Stmt* stmt);
    DataType getExprType(Expr* expr);

    // Specific statement logic
    void handleDeclaration(DeclarationStmt* stmt);
    void handleAssignment(AssignmentStmt* stmt);

    // Logic Helpers
    DataType stringToType(string typeStr);
    bool isCompatible(DataType left, DataType right);
    DataType evaluateBinary(DataType left, DataType right, string op);

    // Error logging logic
    void reportError(string msg, int line);
    void reportWarning(string msg);

public:
    // Constructor: Purani symbol table connect karo
    SemanticAnalyzer(SymbolTable& st) : symTable(st) {}

    // Entry point: Pura program scan karo
    void analyze(vector<Stmt*> program);

    // Final Report: File mein results save karo
    void writeReport(string filePath);
};

#endif