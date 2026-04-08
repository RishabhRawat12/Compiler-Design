#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../parser/syntax_analyzer.h"

struct SymbolEntry {
    std::string name;
    std::string type;
    std::string scope;
    int declarationLine;
    bool isUsed;
    bool isFunction;
    std::vector<std::string> paramTypes;
};

struct SemanticError {
    std::string message;
    int line;
    int column;
    std::string severity;
};

struct SemanticWarning {
    std::string message;
    int line;
    int column;
    std::string category;
};

struct SemanticResult {
    std::vector<SymbolEntry> symbolTable;
    std::vector<SemanticError> errors;
    std::vector<SemanticWarning> warnings;
    int cyclomaticComplexity;
    int maxNestingDepth;
    int functionCount;
};

SemanticResult analyzeSemantics(const std::shared_ptr<ASTNode>& root);
