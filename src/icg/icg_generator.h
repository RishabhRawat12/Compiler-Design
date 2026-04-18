#ifndef ICG_GENERATOR_H
#define ICG_GENERATOR_H

#include <vector>
#include <string>
#include <memory>
#include "tac.h"
#include "../AST/stmt.h"
#include "../AST/expr.h"

using namespace std;

class ICGGenerator {
private:
    vector<TACInstruction> instructions;
    int tempCount;
    int labelCount;

    // Helper functions to get new temps and labels
    string nextTemp();
    string nextLabel();

    // Traversal methods (using logic similar to SemanticAnalyzer)
    void translateStmt(Stmt* stmt);
    string translateExpr(Expr* expr);

    // Specific handlers
    void handleDeclaration(DeclarationStmt* stmt);
    void handleAssignment(AssignmentStmt* stmt);
    void handleIf(IfStmt* stmt);
    void handleFor(ForStmt* stmt);
    void handleFunction(FunctionStmt* stmt);

public:
    ICGGenerator() : tempCount(0), labelCount(0) {}
    void generate(const vector<unique_ptr<Stmt>>& program);
    vector<TACInstruction> getInstructions() { return instructions; }
    void printInstructions();
    string getInstructionsAsString();
};

#endif
