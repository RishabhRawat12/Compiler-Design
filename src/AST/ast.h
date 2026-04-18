#ifndef AST_BASE_H
#define AST_BASE_H

#include <string>
#include <vector>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class Expr {
public:
    int line = 0;
    virtual ~Expr() = default;
    virtual json toJson() const = 0;
};

class Stmt {
public:
    int line = 0;
    virtual ~Stmt() = default;
    virtual json toJson() const = 0;
};

#endif