#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <memory>
#include "json.hpp"
#include "../../symbol_table.h"
#include "../AST/stmt.h"
#include "../AST/expr.h"

using namespace std;
using json = nlohmann::json;

enum DataType { TYPE_INT, TYPE_CHAR, TYPE_STRING, TYPE_VOID, TYPE_UNKNOWN };

struct Error {
    string message;
    int line, col;
};

class SemanticAnalyzer {
private:
    SymbolTable& symTable;
    vector<Error> errors;
    set<string> reportedErrors;

    map<string, bool> isUsed;
    map<string, bool> isInitialized;

    string currentFunctionType = "";
    bool hasReturn = false;
    int loopDepth = 0;

    void visitStmt(Stmt* stmt);
    DataType getExprType(Expr* expr);

    void handleDeclaration(DeclarationStmt* stmt);
    void handleAssignment(AssignmentStmt* stmt);
    void handleFunction(FunctionStmt* stmt);

    DataType stringToType(string);
    bool isCompatible(DataType, DataType);
    DataType evaluateBinary(DataType, DataType, string, int);
    bool isConstantExpression(Expr* expr);
    bool isLValue(Expr* expr);

    void reportError(string msg, int line, int col);
    void reportWarning(string msg);

public:
    SemanticAnalyzer(SymbolTable& st) : symTable(st) {}
    void analyze(const vector<unique_ptr<Stmt>>& program);
    vector<Error> getErrors();
    json getErrorsAsJson();
};

#endif