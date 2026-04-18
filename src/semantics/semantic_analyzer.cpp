#include "semantic_analyzer.h"

void SemanticAnalyzer::analyze(const vector<unique_ptr<Stmt>>& program) {
    for (const auto& stmt : program) if (stmt) visitStmt(stmt.get());
    for (auto const& pair : symTable.table) {
        string name = pair.first;
        if (!pair.second.isFunction && (isUsed.find(name) == isUsed.end() || !isUsed[name])) {
            reportWarning("Unused variable: '" + name + "'");
        }
    }
}

void SemanticAnalyzer::visitStmt(Stmt* stmt) {
    if (auto decl = dynamic_cast<DeclarationStmt*>(stmt)) handleDeclaration(decl);
    else if (auto assign = dynamic_cast<AssignmentStmt*>(stmt)) handleAssignment(assign);
    else if (auto func = dynamic_cast<FunctionStmt*>(stmt)) handleFunction(func);
    else if (auto ret = dynamic_cast<ReturnStmt*>(stmt)) {
        hasReturn = true;
        if (currentFunctionType == "") {
            reportError("Cannot return from outside a function", ret->line, 0);
            return;
        }
        DataType expectedType = stringToType(currentFunctionType);
        DataType actualType = ret->value ? getExprType(ret->value.get()) : TYPE_VOID;
        
        if (expectedType == TYPE_VOID && actualType != TYPE_VOID) reportError("Void function cannot return a value", ret->line, 0);
        else if (expectedType != TYPE_VOID && actualType == TYPE_VOID) reportError("Missing return value", ret->line, 0);
        else if (!isCompatible(expectedType, actualType)) reportError("Return type mismatch", ret->line, 0);
    }
    else if (auto brk = dynamic_cast<BreakStmt*>(stmt)) {
        if (loopDepth == 0) reportError("'break' statement not within loop", brk->line, 0);
    }
    else if (auto cont = dynamic_cast<ContinueStmt*>(stmt)) {
        if (loopDepth == 0) reportError("'continue' statement not within loop", cont->line, 0);
    }
    else if (auto ifs = dynamic_cast<IfStmt*>(stmt)) {
        DataType condType = getExprType(ifs->condition.get());
        if (condType != TYPE_INT) reportError("Condition must be integer type", ifs->line, 0);
        symTable.enterScope();
        for (auto& s : ifs->thenBranch) visitStmt(s.get());
        symTable.exitScope();
        symTable.enterScope();
        for (auto& s : ifs->elseBranch) visitStmt(s.get());
        symTable.exitScope();
    }
    else if (auto fr = dynamic_cast<ForStmt*>(stmt)) {
        symTable.enterScope();
        if (fr->init) visitStmt(fr->init.get());
        if (fr->condition) {
            DataType condType = getExprType(fr->condition.get());
            if (condType != TYPE_INT) reportError("For loop condition must be integer", fr->line, 0);
        }
        if (fr->increment) visitStmt(fr->increment.get());
        loopDepth++;
        for (auto& s : fr->body) visitStmt(s.get());
        loopDepth--;
        symTable.exitScope();
    }
}

void SemanticAnalyzer::handleFunction(FunctionStmt* stmt) {
    vector<string> paramTypes;
    for (auto& p : stmt->params) paramTypes.push_back(p.first);

    if (symTable.lookupCurrentScope(stmt->name)) {
        reportError("Function '" + stmt->name + "' already declared", stmt->line, 0);
        return;
    }

    symTable.insertFunctionWithHistory(stmt->name, stmt->returnType, stmt->line, paramTypes);
    currentFunctionType = stmt->returnType;
    hasReturn = false;

    symTable.enterScope();
    for (auto& p : stmt->params) {
        symTable.insertWithHistory(p.second, p.first, stmt->line);
        isInitialized[p.second] = true;
        isUsed[p.second] = false;
    }
    for (auto& s : stmt->body) visitStmt(s.get());
    
    if (currentFunctionType != "void" && !hasReturn) {
        reportError("Function '" + stmt->name + "' missing return statement", stmt->line, 0);
    }
    symTable.exitScope();
    currentFunctionType = "";
}

bool SemanticAnalyzer::isConstantExpression(Expr* expr) {
    if (expr == nullptr) return true;
    if (dynamic_cast<VariableExpr*>(expr) || dynamic_cast<CallExpr*>(expr)) return false;
    if (auto binExpr = dynamic_cast<BinaryExpr*>(expr)) return isConstantExpression(binExpr->left.get()) && isConstantExpression(binExpr->right.get());
    return true;
}

bool SemanticAnalyzer::isLValue(Expr* expr) {
    if (dynamic_cast<VariableExpr*>(expr)) return true;
    return false;
}

void SemanticAnalyzer::handleDeclaration(DeclarationStmt* stmt) {
    string name = stmt->name;
    if (symTable.lookupCurrentScope(name)) {
        reportError("Variable '" + name + "' already declared in this scope", stmt->line, 0);
        return;
    }
    symTable.insertWithHistory(name, stmt->varType, stmt->line);
    DataType declaredType = stringToType(stmt->varType);
    isUsed[name] = false;

    if (stmt->initializer) {
        if (symTable.get(name).scope == "GLOBAL" && !isConstantExpression(stmt->initializer.get())) {
            reportError("Initializer element is not constant", stmt->line, 0);
            return; 
        }
        DataType exprType = getExprType(stmt->initializer.get());
        if (exprType == TYPE_UNKNOWN) return;
        if (!isCompatible(declaredType, exprType)) reportError("Type mismatch in declaration of '" + name + "'", stmt->line, 0);
        else if (declaredType == TYPE_CHAR && exprType == TYPE_INT) reportWarning("Possible data loss: assigning int to char");
        isInitialized[name] = true;
    } else {
        isInitialized[name] = false;
    }
}

void SemanticAnalyzer::handleAssignment(AssignmentStmt* stmt) {
    if (!isLValue(stmt->target.get())) {
        reportError("Left side of assignment must be a valid memory location (l-value)", stmt->line, 0);
        return;
    }
    auto varExpr = dynamic_cast<VariableExpr*>(stmt->target.get());
    string name = varExpr->name;
    if (!symTable.lookup(name)) {
        reportError("Variable '" + name + "' not declared before assignment", stmt->line, 0);
        return;
    }
    DataType declaredType = stringToType(symTable.get(name).type);
    DataType exprType = getExprType(stmt->value.get());
    if (exprType == TYPE_UNKNOWN) return;
    if (!isCompatible(declaredType, exprType)) reportError("Type mismatch in assignment", stmt->line, 0);
    else if (declaredType == TYPE_CHAR && exprType == TYPE_INT) reportWarning("Possible data loss: assigning int to char");
    isInitialized[name] = true;
}

DataType SemanticAnalyzer::getExprType(Expr* expr) {
    if (dynamic_cast<NumberExpr*>(expr)) return TYPE_INT;
    if (dynamic_cast<StringExpr*>(expr)) return TYPE_STRING;
    if (dynamic_cast<CharExpr*>(expr)) return TYPE_CHAR;

    if (auto varExpr = dynamic_cast<VariableExpr*>(expr)) {
        string name = varExpr->name;
        if (!symTable.lookup(name)) {
            reportError("Undeclared variable '" + name + "'", varExpr->line, 0);
            return TYPE_UNKNOWN;
        }
        isUsed[name] = true;
        if (!isInitialized[name]) reportWarning("Variable '" + name + "' used before initialization");
        return stringToType(symTable.get(name).type);
    }

    if (auto call = dynamic_cast<CallExpr*>(expr)) {
        if (!symTable.lookup(call->callee)) {
            reportError("Undeclared function '" + call->callee + "'", call->line, 0);
            return TYPE_UNKNOWN;
        }
        SymbolInfo info = symTable.get(call->callee);
        if (!info.isFunction) {
            reportError("'" + call->callee + "' is not a function", call->line, 0);
            return TYPE_UNKNOWN;
        }
        if (info.paramTypes.size() != call->arguments.size()) {
            reportError("Argument count mismatch for '" + call->callee + "'", call->line, 0);
            return TYPE_UNKNOWN;
        }
        for (size_t i = 0; i < call->arguments.size(); i++) {
            DataType argType = getExprType(call->arguments[i].get());
            DataType expectedType = stringToType(info.paramTypes[i]);
            if (!isCompatible(expectedType, argType)) {
                reportError("Type mismatch in argument " + to_string(i+1) + " of '" + call->callee + "'", call->line, 0);
            }
        }
        return stringToType(info.type);
    }

    if (auto binExpr = dynamic_cast<BinaryExpr*>(expr)) {
        DataType leftType = getExprType(binExpr->left.get());
        DataType rightType = getExprType(binExpr->right.get());
        if (leftType == TYPE_UNKNOWN || rightType == TYPE_UNKNOWN) return TYPE_UNKNOWN;
        return evaluateBinary(leftType, rightType, binExpr->op, binExpr->line);
    }
    return TYPE_UNKNOWN;
}

DataType SemanticAnalyzer::evaluateBinary(DataType left, DataType right, string op, int line) {
    if (op == "+") {
        if (left == TYPE_INT && right == TYPE_INT) return TYPE_INT;
        if (left == TYPE_STRING && right == TYPE_STRING) return TYPE_STRING;
        reportError("Invalid arithmetic operation", line, 0);
        return TYPE_UNKNOWN;
    }
    if (op == "-" || op == "*" || op == "/") {
        if (left == TYPE_INT && right == TYPE_INT) return TYPE_INT;
        reportError("Invalid arithmetic operation", line, 0);
        return TYPE_UNKNOWN;
    }
    if (op == ">" || op == "<" || op == ">=" || op == "<=" || op == "==" || op == "!=") {
        if (left == right) return TYPE_INT;
        reportError("Invalid comparison between incompatible types", line, 0);
        return TYPE_UNKNOWN;
    }
    return TYPE_UNKNOWN;
}

bool SemanticAnalyzer::isCompatible(DataType a, DataType b) {
    if (a == b) return true;
    if (a == TYPE_INT && b == TYPE_CHAR) return true;
    if (a == TYPE_CHAR && b == TYPE_INT) return true;
    return false;
}

DataType SemanticAnalyzer::stringToType(string typeStr) {
    if (typeStr == "int") return TYPE_INT;
    if (typeStr == "char") return TYPE_CHAR;
    if (typeStr == "string") return TYPE_STRING;
    if (typeStr == "void") return TYPE_VOID;
    return TYPE_UNKNOWN;
}

void SemanticAnalyzer::reportError(string msg, int line, int col) {
    string key = msg + to_string(line) + to_string(col);
    if (reportedErrors.find(key) == reportedErrors.end()) {
        reportedErrors.insert(key);
        errors.push_back({msg, line, col});
    }
}

void SemanticAnalyzer::reportWarning(string msg) {
    errors.push_back({"WARNING: " + msg, 0, 0});
}

vector<Error> SemanticAnalyzer::getErrors() { return errors; }

json SemanticAnalyzer::getErrorsAsJson() {
    json arr = json::array();
    for (auto& e : errors) {
        json obj;
        obj["line"] = e.line;
        obj["col"] = e.col;
        obj["message"] = e.message;
        obj["type"] = (e.message.find("WARNING:") != string::npos) ? "warning" : "error";
        arr.push_back(obj);
    }
    return arr;
}