#include "semantic_analyzer.h"
#include <algorithm>

struct Scope {
    std::string name;
    std::map<std::string, SymbolEntry> symbols;
    Scope* parent;
    Scope(const std::string& n, Scope* p) : name(n), parent(p) {}

    SymbolEntry* lookup(const std::string& id) {
        auto it = symbols.find(id);
        if (it != symbols.end()) return &it->second;
        if (parent) return parent->lookup(id);
        return nullptr;
    }
};

struct SemanticAnalyzer {
    Scope* currentScope;
    SemanticResult result;
    int tempCounter = 0;
    int nestingDepth = 0;
    int maxNesting = 0;
    std::string currentFunction;
    std::map<std::string, int> functionLineCounts;

    SemanticAnalyzer() {
        currentScope = new Scope("global", nullptr);
        result.cyclomaticComplexity = 1;
        result.maxNestingDepth = 0;
        result.functionCount = 0;
    }

    void pushScope(const std::string& name) {
        currentScope = new Scope(name, currentScope);
        nestingDepth++;
        if (nestingDepth > maxNesting) maxNesting = nestingDepth;
    }

    void popScope() {
        // Check for unused variables before popping
        for (auto& [name, sym] : currentScope->symbols) {
            if (!sym.isUsed && !sym.isFunction) {
                result.warnings.push_back({
                    "Variable '" + name + "' is declared but never used",
                    sym.declarationLine, 0, "unused-variable"
                });
            }
        }
        Scope* parent = currentScope->parent;
        delete currentScope;
        currentScope = parent;
        nestingDepth--;
    }

    void declare(const std::string& name, const std::string& type, int line, bool isFunc = false) {
        if (currentScope->symbols.count(name)) {
            result.errors.push_back({"Variable '" + name + "' already declared in this scope", line, 0, "error"});
            return;
        }
        SymbolEntry entry;
        entry.name = name;
        entry.type = type;
        entry.scope = currentScope->name;
        entry.declarationLine = line;
        entry.isUsed = false;
        entry.isFunction = isFunc;
        currentScope->symbols[name] = entry;
        result.symbolTable.push_back(entry);
    }

    std::string analyzeNode(const std::shared_ptr<ASTNode>& node) {
        if (!node) return "void";

        if (node->type == "Program") {
            for (auto& child : node->children) analyzeNode(child);
        }
        else if (node->type == "FunctionDef") {
            result.functionCount++;
            std::string retType = "void";
            for (auto& child : node->children) {
                if (child->type == "ReturnType") retType = child->value;
            }
            declare(node->value, retType, node->line, true);
            currentFunction = node->value;
            pushScope("function:" + node->value);
            for (auto& child : node->children) {
                if (child->type != "ReturnType" && child->type != "ParamList") continue;
                if (child->type == "ParamList") {
                    for (auto& param : child->children) {
                        if (!param->children.empty()) {
                            declare(param->value, param->children[0]->value, param->line);
                        }
                    }
                }
            }
            for (auto& child : node->children) {
                if (child->type == "Block") analyzeNode(child);
            }
            popScope();
        }
        else if (node->type == "Declaration") {
            std::string type = "int";
            for (auto& child : node->children) {
                if (child->type == "Type") type = child->value;
            }
            for (auto& child : node->children) {
                if (child->type == "VarDecl") {
                    declare(child->value, type, child->line);
                    if (!child->children.empty()) analyzeNode(child->children[0]);
                }
            }
        }
        else if (node->type == "Block") {
            pushScope("block");
            for (auto& child : node->children) analyzeNode(child);
            popScope();
        }
        else if (node->type == "If" || node->type == "While" || node->type == "For" || node->type == "DoWhile") {
            result.cyclomaticComplexity++;
            for (auto& child : node->children) analyzeNode(child);
        }
        else if (node->type == "Identifier") {
            auto* sym = currentScope->lookup(node->value);
            if (!sym) {
                result.errors.push_back({"Undeclared identifier '" + node->value + "'", node->line, node->column, "error"});
            } else {
                sym->isUsed = true;
                // Update in symbolTable too
                for (auto& s : result.symbolTable) {
                    if (s.name == node->value && s.scope == sym->scope) s.isUsed = true;
                }
                return sym->type;
            }
        }
        else if (node->type == "Assign") {
            std::string rtype = "void";
            if (node->children.size() >= 2) {
                analyzeNode(node->children[0]);
                rtype = analyzeNode(node->children[1]);
            }
            return rtype;
        }
        else if (node->type == "BinaryOp") {
            std::string ltype = "void", rtype = "void";
            if (node->children.size() >= 2) {
                ltype = analyzeNode(node->children[0]);
                rtype = analyzeNode(node->children[1]);
            }
            if (ltype != rtype && ltype != "void" && rtype != "void") {
                result.warnings.push_back({
                    "Implicit type conversion in expression",
                    node->line, node->column, "type-mismatch"
                });
            }
            return ltype;
        }
        else if (node->type == "FuncCall") {
            std::string funcName = node->value;
            // skip printf, scanf as builtins
            if (funcName != "printf" && funcName != "scanf" && funcName != "malloc" &&
                funcName != "free" && funcName != "sizeof") {
                auto* sym = currentScope->lookup(funcName);
                if (!sym) {
                    result.errors.push_back({"Undeclared function '" + funcName + "'", node->line, node->column, "error"});
                } else {
                    sym->isUsed = true;
                    for (auto& s : result.symbolTable) {
                        if (s.name == funcName) s.isUsed = true;
                    }
                    return sym->type;
                }
            }
        }
        else if (node->type == "Return") {
            if (!node->children.empty()) return analyzeNode(node->children[0]);
        }
        else if (node->type == "IntLiteral") return "int";
        else if (node->type == "FloatLiteral") return "float";
        else if (node->type == "CharLiteral") return "char";
        else if (node->type == "StringLiteral") return "char*";
        else {
            for (auto& child : node->children) analyzeNode(child);
        }
        return "void";
    }

    void checkDeepNesting(const std::shared_ptr<ASTNode>& node, int depth = 0) {
        if (!node) return;
        if (depth > 4) {
            result.warnings.push_back({
                "Deep nesting detected (depth " + std::to_string(depth) + ") - consider refactoring",
                node->line, node->column, "deep-nesting"
            });
        }
        bool isControl = (node->type == "If" || node->type == "While" ||
                          node->type == "For" || node->type == "DoWhile" || node->type == "Block");
        for (auto& child : node->children) checkDeepNesting(child, isControl ? depth + 1 : depth);
    }

    SemanticResult analyze(const std::shared_ptr<ASTNode>& root) {
        analyzeNode(root);
        checkDeepNesting(root);
        result.maxNestingDepth = maxNesting;
        return result;
    }
};

SemanticResult analyzeSemantics(const std::shared_ptr<ASTNode>& root) {
    SemanticAnalyzer analyzer;
    return analyzer.analyze(root);
}
