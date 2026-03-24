#ifndef STMT_H
#define STMT_H

#include "ast.h"
#include "expr.h"

// int x = 5; wali logic
class DeclarationStmt : public Stmt {
public:
    string name;
    Expr* initializer; // Pointer 

    DeclarationStmt(string n, Expr* init) : name(n), initializer(init) {}

    ~DeclarationStmt() {
        delete initializer; 
    }

    void print(int indent) override {
        for(int i=0; i<indent; i++) cout << "  ";
        cout << "DeclarationStmt" << endl;
        
        for(int i=0; i<indent+1; i++) cout << "  ";
        cout << "name: " << name << endl;
        
        for(int i=0; i<indent+1; i++) cout << "  ";
        cout << "value:" << endl;
        if(initializer) initializer->print(indent + 2);
    }
};

// x = 10; wali logic
class AssignmentStmt : public Stmt {
public:
    string name;
    Expr* value;

    AssignmentStmt(string n, Expr* v) : name(n), value(v) {}

    ~AssignmentStmt() {
        delete value;
    }
    
    void print(int indent) override {
        for(int i=0; i<indent; i++) cout << "  ";
        cout << "AssignmentStmt" << endl;
        
        for(int i=0; i<indent+1; i++) cout << "  ";
        cout << "name: " << name << endl;
        
        for(int i=0; i<indent+1; i++) cout << "  ";
        cout << "value:" << endl;
        if(value) value->print(indent + 2);
    }
};

#endif
