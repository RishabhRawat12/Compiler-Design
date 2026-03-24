#include "semantic_analyzer.h"

// STEP 2: Entry Function - Pura program scan karo
void SemanticAnalyzer::analyze(vector<Stmt*> program) {
    for (Stmt* stmt : program) {
        if (stmt != nullptr) {
            visitStmt(stmt);
        }
    }

    // Traversal ke baad: Check karo kaunse variables declare huye par kabhi use nahi huye
    for (auto const pair : symTable.table) {
        string name =pair.first;
        Symbol sym=pair.second;
        if (isUsed.find(name) == isUsed.end() || !isUsed[name]) {
            reportWarning("Unused variable: '" + name + "'");
        }
    }
}

// STEP 3: Statement Dispatcher - Routing logic
void SemanticAnalyzer::visitStmt(Stmt* stmt) {
    if (auto decl = dynamic_cast<DeclarationStmt*>(stmt)) {
        handleDeclaration(decl);
    } 
    else if (auto assign = dynamic_cast<AssignmentStmt*>(stmt)) {
        handleAssignment(assign);
    }
}

// STEP 4: Declaration Handling
void SemanticAnalyzer::handleDeclaration(DeclarationStmt* stmt) {
    string name = stmt->name;
    DataType declaredType = stringToType(symTable.table[name].type);
    
    isUsed[name] = false; // Freshly declared, use nahi hua abhi

    if (stmt->initializer != nullptr) {
        DataType exprType = getExprType(stmt->initializer);

        if (exprType == TYPE_UNKNOWN) return; // Error propagation stop karo

        if (!isCompatible(declaredType, exprType)) {
            reportError("Type mismatch in declaration of '" + name + "'", symTable.table[name].line);
        }
        isInitialized[name] = true;
    } else {
        isInitialized[name] = false;
    }
}

// STEP 5: Assignment Handling
void SemanticAnalyzer::handleAssignment(AssignmentStmt* stmt) {
    string name = stmt->name;

    if (!symTable.lookup(name)) {
        reportError("Variable '" + name + "' not declared before assignment", 0);
        return;
    }

    DataType declaredType = stringToType(symTable.table[name].type);
    DataType exprType = getExprType(stmt->value);

    if (exprType == TYPE_UNKNOWN) return;

    if (!isCompatible(declaredType, exprType)) {
        reportError("Type mismatch in assignment to '" + name + "'", symTable.table[name].line);
    }
    isInitialized[name] = true; // Ab value aa gayi variable mein
}

// STEP 6: Expression Type Evaluation (The Brain)
DataType SemanticAnalyzer::getExprType(Expr* expr) {
    if (dynamic_cast<NumberExpr*>(expr)) return TYPE_INT;
    if (dynamic_cast<StringExpr*>(expr)) return TYPE_STRING;
    if (dynamic_cast<CharExpr*>(expr)) return TYPE_CHAR;

    if (auto varExpr = dynamic_cast<VariableExpr*>(expr)) {
        string name = varExpr->name;

        if (!symTable.lookup(name)) {
            reportError("Undeclared variable '" + name + "' used in expression", 0);
            return TYPE_UNKNOWN;
        }

        isUsed[name] = true;
        if (!isInitialized[name]) {
            reportWarning("Variable '" + name + "' used before being initialized");
        }

        return stringToType(symTable.table[name].type);
    }

    if (auto binExpr = dynamic_cast<BinaryExpr*>(expr)) {
        DataType leftType = getExprType(binExpr->left);
        DataType rightType = getExprType(binExpr->right);

        if (leftType == TYPE_UNKNOWN || rightType == TYPE_UNKNOWN) {
            return TYPE_UNKNOWN;
        }

        return evaluateBinary(leftType, rightType, binExpr->op);
    }

    return TYPE_UNKNOWN;
}

// STEP 7: Binary Expression Rules
DataType SemanticAnalyzer::evaluateBinary(DataType left, DataType right, string op) {
    if (op == "+") {
        if (left == TYPE_INT && right == TYPE_INT) return TYPE_INT;
        if (left == TYPE_STRING && right == TYPE_STRING) return TYPE_STRING; // Concatenation rule
        
        reportError("Invalid '+' operation between incompatible types", 0);
        return TYPE_UNKNOWN;
    }

    if (op == "-" || op == "*" || op == "/") {
        if (left == TYPE_INT && right == TYPE_INT) return TYPE_INT;
        
        reportError("Arithmetic operator '" + op + "' only works with integers", 0);
        return TYPE_UNKNOWN;
    }

    return TYPE_UNKNOWN;
}

// STEP 8: Type Compatibility Logic
bool SemanticAnalyzer::isCompatible(DataType a, DataType b) {
    if (a == b) return true;
    
    // Engineering Trick: Char ko int mein promote hone do (ASCII support)
    if (a == TYPE_INT && b == TYPE_CHAR) return true;

    return false;
}

// STEP 9: String to Enum Conversion
DataType SemanticAnalyzer::stringToType(string typeStr) {
    if (typeStr == "KEYWORD_INT") return TYPE_INT;
    if (typeStr == "KEYWORD_CHAR") return TYPE_CHAR;
    if (typeStr == "KEYWORD_STRING") return TYPE_STRING;
    return TYPE_UNKNOWN;
}

// STEP 10: Error Reporting (No Duplicates)
void SemanticAnalyzer::reportError(string msg, int line) {
    string key = msg + to_string(line);
    if (reportedErrors.find(key) == reportedErrors.end()) {
        reportedErrors.insert(key);
        errors.push_back({msg, line});
    }
}

// STEP 11: Warning Reporting
void SemanticAnalyzer::reportWarning(string msg) {
    errors.push_back({"WARNING: " + msg, 0});
}

// STEP 12: Final Report Generation
void SemanticAnalyzer::writeReport(string filePath) {
    ofstream file(filePath);
    if (!file.is_open()) {
        cerr << "Failed to open " << filePath << " for writing report!" << endl;
        return;
    }

    file << "=== SEMANTIC ANALYSIS REPORT ===\n";
    symTable.display(file); // Purani symbol table ka snapshot

    file << "\n--- ERRORS ---\n";
    bool hasErrors = false;
    for (const auto& err : errors) {
        if (err.message.find("WARNING:") == string::npos) {
            file << "[Line " << err.line << "] Error: " << err.message << "\n";
            hasErrors = true;
        }
    }
    if (!hasErrors) file << "None found.\n";

    file << "\n--- WARNINGS ---\n";
    bool hasWarnings = false;
    for (const auto& err : errors) {
        if (err.message.find("WARNING:") != string::npos) {
            file << err.message << "\n";
            hasWarnings = true;
        }
    }
    if (!hasWarnings) file << "None found.\n";

    file.close();
}