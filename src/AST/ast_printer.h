#ifndef AST_PRINTER_H
#define AST_PRINTER_H

#include "stmt.h"
#include <vector>

class ASTPrinter {
public:
    // Pura AST (list of statements) print karne ke liye
    static void printAST(const vector<Stmt*>& program) {
        cout << "\n--- Generated AST Structure ---" << endl;
        for (Stmt* stmt : program) {
            if (stmt) {
                stmt->print(0); // Level 0 indentation se start karo
                cout << endl;
            }
        }
    }
};

#endif
