#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../lexer/lexical_analyzer.h"

struct ASTNode {
    std::string type;
    std::string value;
    int line;
    int column;
    std::vector<std::shared_ptr<ASTNode>> children;
};

struct SyntaxError {
    std::string message;
    int line;
    int column;
};

struct ParseResult {
    std::shared_ptr<ASTNode> root;
    std::vector<SyntaxError> errors;
};

ParseResult parse(const std::vector<Token>& tokens);
std::string astToJson(const std::shared_ptr<ASTNode>& node, int indent = 0);
