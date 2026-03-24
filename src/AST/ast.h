#ifndef AST_BASE_H
#define AST_BASE_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// Base class for all expressions (like 5, x, etc.)
class Expr {
public:
    virtual ~Expr() {}
    // pure virtual function - printer isko use karega
    virtual void print(int indent) = 0; 
};

// Base class for all statements (like int x = 5;)
class Stmt {
public:
    virtual ~Stmt() {}
    virtual void print(int indent) = 0;
};

#endif
