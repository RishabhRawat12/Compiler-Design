#ifndef EXPR_H
#define EXPR_H

#include "ast.h"

//adding number 
class NumberExpr : public Expr {
public:
    int value;
    NumberExpr(int val) : value(val) {}
    void print(int indent) override {
        for(int i=0; i<indent; i++) cout << "  ";
        cout << "NumberExpr(" << value << ")" << endl;
    }
};


//adding variables
class VariableExpr : public Expr {
public:
    string name;
    VariableExpr(string n) : name(n) {}
    void print(int indent) override {
        for(int i=0; i<indent; i++) cout << "  ";
        cout << "VariableExpr(" << name << ")" << endl;
    }
};

//adding string literals
class StringExpr : public Expr {
public:
    string value;
    StringExpr(string v) : value(v) {}
    void print(int indent) override {
        for(int i=0; i<indent; i++) cout << "  ";
        cout << "StringExpr(" << value << ")" << endl;
    }
};

//Added Char Literals Node
class CharExpr : public Expr {
public:
    string value;
    CharExpr(string v) : value(v) {}
    void print(int indent) override {
        for(int i=0; i<indent; i++) cout << "  ";
        cout << "CharExpr(" << value << ")" << endl;
    }
};


//node  for binary operations (+, -, *, /) ke liye
class BinaryExpr : public Expr {
public:
    Expr* left;
    string op;
    Expr* right;

    BinaryExpr(Expr* l, string o, Expr* r) : left(l), op(o), right(r) {}

    // Destructor 
    ~BinaryExpr() {
        delete left;
        delete right;
    }

    void print(int indent) override {
        for(int i=0; i<indent; i++) cout << "  ";
        cout << "BinaryExpr(" << op << ")" << endl;
        
        for(int i=0; i<indent+1; i++) cout << "  ";
        cout << "left:" << endl;
        if(left) left->print(indent + 2);
        
        for(int i=0; i<indent+1; i++) cout << "  ";
        cout << "right:" << endl;
        if(right) right->print(indent + 2);
    }
};

#endif
