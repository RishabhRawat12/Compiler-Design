#pragma once

#include <vector>
#include <memory>
#include "json.hpp"

using json = nlohmann::json;

class Stmt;

class ASTPrinter {
public:
    // Simple wrapper: AST → JSON array
    static json buildAST(const std::vector<std::unique_ptr<Stmt>>& program);
};
