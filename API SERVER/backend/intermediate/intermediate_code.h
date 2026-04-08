#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../parser/syntax_analyzer.h"

struct TACInstruction {
    std::string op;
    std::string result;
    std::string arg1;
    std::string arg2;
    std::string label;
    int lineNumber;

    std::string toString() const;
};

struct IRResult {
    std::vector<TACInstruction> instructions;
};

IRResult generateIR(const std::shared_ptr<ASTNode>& root);
