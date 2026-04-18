#include "icg_generator.h"
#include <iostream>

string ICGGenerator::nextTemp() {
    return "t" + to_string(tempCount++);
}

string ICGGenerator::nextLabel() {
    return "L" + to_string(labelCount++);
}

void ICGGenerator::generate(const vector<unique_ptr<Stmt>>& program) {
    for (const auto& stmt : program) {
        if (stmt) translateStmt(stmt.get());
    }
}

void ICGGenerator::translateStmt(Stmt* stmt) {
    if (auto decl = dynamic_cast<DeclarationStmt*>(stmt)) {
        handleDeclaration(decl);
    } else if (auto assign = dynamic_cast<AssignmentStmt*>(stmt)) {
        handleAssignment(assign);
    } else if (auto ifs = dynamic_cast<IfStmt*>(stmt)) {
        handleIf(ifs);
    } else if (auto fr = dynamic_cast<ForStmt*>(stmt)) {
        handleFor(fr);
    } else if (auto func = dynamic_cast<FunctionStmt*>(stmt)) {
        handleFunction(func);
    } else if (auto ret = dynamic_cast<ReturnStmt*>(stmt)) {
        string val = ret->value ? translateExpr(ret->value.get()) : "";
        instructions.push_back(TACInstruction("RETURN", val, "", ""));
    } else if (auto brk = dynamic_cast<BreakStmt*>(stmt)) {
        // Break/Continue would need label tracking for the current loop
        // Keeping it simple for now as it's a student project
        instructions.push_back(TACInstruction("GOTO", "", "", "LOOP_END")); 
    }
}

void ICGGenerator::handleDeclaration(DeclarationStmt* stmt) {
    if (stmt->initializer) {
        string src = translateExpr(stmt->initializer.get());
        instructions.push_back(TACInstruction("=", src, "", stmt->name));
    }
}

void ICGGenerator::handleAssignment(AssignmentStmt* stmt) {
    string src = translateExpr(stmt->value.get());
    string dest = "";
    if (auto var = dynamic_cast<VariableExpr*>(stmt->target.get())) {
        dest = var->name;
    }
    if (dest != "") {
        instructions.push_back(TACInstruction("=", src, "", dest));
    }
}

void ICGGenerator::handleIf(IfStmt* stmt) {
    string cond = translateExpr(stmt->condition.get());
    string elseLabel = nextLabel();
    string endLabel = nextLabel();

    // if !cond goto elseLabel
    instructions.push_back(TACInstruction("IF", cond, "== 0", elseLabel));

    for (auto& s : stmt->thenBranch) translateStmt(s.get());
    instructions.push_back(TACInstruction("GOTO", "", "", endLabel));

    instructions.push_back(TACInstruction("LABEL", "", "", elseLabel));
    for (auto& s : stmt->elseBranch) translateStmt(s.get());
    
    instructions.push_back(TACInstruction("LABEL", "", "", endLabel));
}

void ICGGenerator::handleFor(ForStmt* stmt) {
    if (stmt->init) translateStmt(stmt->init.get());
    
    string startLabel = nextLabel();
    string endLabel = nextLabel();

    instructions.push_back(TACInstruction("LABEL", "", "", startLabel));

    if (stmt->condition) {
        string cond = translateExpr(stmt->condition.get());
        instructions.push_back(TACInstruction("IF", cond, "== 0", endLabel));
    }

    for (auto& s : stmt->body) translateStmt(s.get());

    if (stmt->increment) translateStmt(stmt->increment.get());
    instructions.push_back(TACInstruction("GOTO", "", "", startLabel));

    instructions.push_back(TACInstruction("LABEL", "", "", endLabel));
}

void ICGGenerator::handleFunction(FunctionStmt* stmt) {
    instructions.push_back(TACInstruction("BEGIN_FUNC", "", "", stmt->name));
    for (auto& s : stmt->body) translateStmt(s.get());
    instructions.push_back(TACInstruction("END_FUNC", "", "", ""));
}

string ICGGenerator::translateExpr(Expr* expr) {
    if (auto num = dynamic_cast<NumberExpr*>(expr)) {
        return to_string(num->value);
    }
    if (auto var = dynamic_cast<VariableExpr*>(expr)) {
        return var->name;
    }
    if (auto str = dynamic_cast<StringExpr*>(expr)) {
        return "\"" + str->value + "\"";
    }
    if (auto ch = dynamic_cast<CharExpr*>(expr)) {
        return "'" + ch->value + "'";
    }
    if (auto bin = dynamic_cast<BinaryExpr*>(expr)) {
        string left = translateExpr(bin->left.get());
        string right = translateExpr(bin->right.get());
        string temp = nextTemp();
        instructions.push_back(TACInstruction(bin->op, left, right, temp));
        return temp;
    }
    if (auto call = dynamic_cast<CallExpr*>(expr)) {
        vector<string> args;
        for (auto& arg : call->arguments) {
            args.push_back(translateExpr(arg.get()));
        }
        for (const auto& a : args) {
            instructions.push_back(TACInstruction("PARAM", a, "", ""));
        }
        string temp = nextTemp();
        instructions.push_back(TACInstruction("CALL", call->callee, to_string(args.size()), temp));
        return temp;
    }
    return "";
}

string ICGGenerator::getInstructionsAsString() {
    stringstream ss;
    for (const auto& inst : instructions) {
        ss << inst.toString() << "\n";
    }
    return ss.str();
}

void ICGGenerator::printInstructions() {
    cout << getInstructionsAsString();
}
