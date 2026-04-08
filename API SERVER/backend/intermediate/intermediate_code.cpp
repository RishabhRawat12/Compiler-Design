#include "intermediate_code.h"
#include <sstream>

struct IRGenerator {
    std::vector<TACInstruction> instructions;
    int tempCount = 0;
    int labelCount = 0;
    int lineNum = 1;

    std::string newTemp() { return "t" + std::to_string(tempCount++); }
    std::string newLabel() { return "L" + std::to_string(labelCount++); }

    void emit(const std::string& op, const std::string& result,
              const std::string& arg1 = "", const std::string& arg2 = "",
              const std::string& label = "") {
        instructions.push_back({op, result, arg1, arg2, label, lineNum++});
    }

    void emitLabel(const std::string& label) {
        instructions.push_back({"label", "", "", "", label, lineNum++});
    }

    std::string generateExpr(const std::shared_ptr<ASTNode>& node) {
        if (!node) return "";

        if (node->type == "IntLiteral" || node->type == "FloatLiteral" || node->type == "CharLiteral") {
            return node->value;
        }
        if (node->type == "StringLiteral") {
            return node->value;
        }
        if (node->type == "Identifier") {
            return node->value;
        }
        if (node->type == "Paren" && !node->children.empty()) {
            return generateExpr(node->children[0]);
        }
        if (node->type == "BinaryOp" && node->children.size() >= 2) {
            std::string left = generateExpr(node->children[0]);
            std::string right = generateExpr(node->children[1]);
            std::string temp = newTemp();
            emit(node->value, temp, left, right);
            return temp;
        }
        if (node->type == "UnaryOp" && !node->children.empty()) {
            std::string operand = generateExpr(node->children[0]);
            std::string temp = newTemp();
            emit("unary" + node->value, temp, operand);
            return temp;
        }
        if (node->type == "PostfixOp" && !node->children.empty()) {
            std::string operand = generateExpr(node->children[0]);
            std::string temp = newTemp();
            emit("=", temp, operand);
            emit(node->value == "++" ? "+" : "-", operand, operand, "1");
            return temp;
        }
        if (node->type == "Assign" && node->children.size() >= 2) {
            std::string rhs = generateExpr(node->children[1]);
            std::string lhs = generateExpr(node->children[0]);
            if (node->value == "=") {
                emit("=", lhs, rhs);
            } else {
                // compound assignment: lhs op= rhs => temp = lhs op rhs; lhs = temp
                std::string baseOp = node->value.substr(0, node->value.size() - 1);
                std::string temp = newTemp();
                emit(baseOp, temp, lhs, rhs);
                emit("=", lhs, temp);
            }
            return lhs;
        }
        if (node->type == "FuncCall") {
            // push arguments
            for (size_t i = 1; i < node->children.size(); i++) {
                std::string arg = generateExpr(node->children[i]);
                emit("param", "", arg);
            }
            std::string temp = newTemp();
            std::string funcName = node->value;
            if (funcName.empty() && !node->children.empty()) {
                funcName = node->children[0]->value;
            }
            emit("call", temp, funcName, std::to_string(node->children.size() > 0 ? node->children.size() - 1 : 0));
            return temp;
        }
        if (node->type == "ArrayAccess" && node->children.size() >= 2) {
            std::string arr = generateExpr(node->children[0]);
            std::string idx = generateExpr(node->children[1]);
            std::string temp = newTemp();
            emit("[]", temp, arr, idx);
            return temp;
        }
        if (node->type == "Ternary" && node->children.size() >= 3) {
            std::string cond = generateExpr(node->children[0]);
            std::string falseLabel = newLabel();
            std::string endLabel = newLabel();
            std::string temp = newTemp();
            emit("iffalse", "", cond, "", falseLabel);
            std::string trueVal = generateExpr(node->children[1]);
            emit("=", temp, trueVal);
            emit("goto", "", "", "", endLabel);
            emitLabel(falseLabel);
            std::string falseVal = generateExpr(node->children[2]);
            emit("=", temp, falseVal);
            emitLabel(endLabel);
            return temp;
        }
        return "";
    }

    void generateStmt(const std::shared_ptr<ASTNode>& node) {
        if (!node) return;

        if (node->type == "Program") {
            for (auto& child : node->children) generateStmt(child);
        }
        else if (node->type == "FunctionDef") {
            emitLabel("func_" + node->value);
            emit("begin_func", node->value);
            for (auto& child : node->children) {
                if (child->type == "Block") generateStmt(child);
            }
            emit("end_func", node->value);
        }
        else if (node->type == "Block") {
            for (auto& child : node->children) generateStmt(child);
        }
        else if (node->type == "Declaration") {
            std::string type = "int";
            for (auto& child : node->children) {
                if (child->type == "Type") type = child->value;
            }
            for (auto& child : node->children) {
                if (child->type == "VarDecl") {
                    emit("decl", child->value, type);
                    if (!child->children.empty()) {
                        std::string val = generateExpr(child->children[0]);
                        emit("=", child->value, val);
                    }
                }
            }
        }
        else if (node->type == "ExprStmt") {
            if (!node->children.empty()) generateExpr(node->children[0]);
        }
        else if (node->type == "If") {
            std::string falseLabel = newLabel();
            std::string endLabel = newLabel();
            if (node->children.size() >= 1) {
                std::string cond = generateExpr(node->children[0]);
                emit("iffalse", "", cond, "", falseLabel);
            }
            if (node->children.size() >= 2) generateStmt(node->children[1]);
            if (node->children.size() >= 3) {
                emit("goto", "", "", "", endLabel);
                emitLabel(falseLabel);
                generateStmt(node->children[2]);
                emitLabel(endLabel);
            } else {
                emitLabel(falseLabel);
            }
        }
        else if (node->type == "While") {
            std::string startLabel = newLabel();
            std::string endLabel = newLabel();
            emitLabel(startLabel);
            if (!node->children.empty()) {
                std::string cond = generateExpr(node->children[0]);
                emit("iffalse", "", cond, "", endLabel);
            }
            if (node->children.size() >= 2) generateStmt(node->children[1]);
            emit("goto", "", "", "", startLabel);
            emitLabel(endLabel);
        }
        else if (node->type == "For") {
            std::string startLabel = newLabel();
            std::string endLabel = newLabel();
            if (node->children.size() >= 1 && node->children[0]->type != "Empty") generateStmt(node->children[0]);
            emitLabel(startLabel);
            if (node->children.size() >= 2 && node->children[1]->type != "Empty") {
                std::string cond = generateExpr(node->children[1]);
                emit("iffalse", "", cond, "", endLabel);
            }
            if (node->children.size() >= 4) generateStmt(node->children[3]);
            if (node->children.size() >= 3 && node->children[2]->type != "Empty") generateExpr(node->children[2]);
            emit("goto", "", "", "", startLabel);
            emitLabel(endLabel);
        }
        else if (node->type == "DoWhile") {
            std::string startLabel = newLabel();
            emitLabel(startLabel);
            if (!node->children.empty()) generateStmt(node->children[0]);
            if (node->children.size() >= 2) {
                std::string cond = generateExpr(node->children[1]);
                emit("iftrue", "", cond, "", startLabel);
            }
        }
        else if (node->type == "Return") {
            if (!node->children.empty()) {
                std::string val = generateExpr(node->children[0]);
                emit("return", "", val);
            } else {
                emit("return", "");
            }
        }
        else if (node->type == "Break") { emit("goto", "", "", "", "break"); }
        else if (node->type == "Continue") { emit("goto", "", "", "", "continue"); }
    }

    IRResult run(const std::shared_ptr<ASTNode>& root) {
        generateStmt(root);
        return {instructions};
    }
};

std::string TACInstruction::toString() const {
    std::ostringstream ss;
    if (!label.empty() && op == "label") {
        ss << label << ":";
        return ss.str();
    }
    if (!label.empty()) {
        ss << op << " " << result << " goto " << label;
        return ss.str();
    }
    if (op == "param") { ss << "param " << arg1; return ss.str(); }
    if (op == "call") { ss << result << " = call " << arg1 << ", " << arg2; return ss.str(); }
    if (op == "return") { ss << "return " << arg1; return ss.str(); }
    if (op == "decl") { ss << arg1 << " " << result; return ss.str(); }
    if (op == "begin_func") { ss << "begin " << result; return ss.str(); }
    if (op == "end_func") { ss << "end " << result; return ss.str(); }
    if (op == "=") {
        ss << result << " = " << arg1;
        return ss.str();
    }
    if (arg2.empty()) {
        ss << result << " = " << op << " " << arg1;
    } else {
        ss << result << " = " << arg1 << " " << op << " " << arg2;
    }
    return ss.str();
}

IRResult generateIR(const std::shared_ptr<ASTNode>& root) {
    IRGenerator gen;
    return gen.run(root);
}
