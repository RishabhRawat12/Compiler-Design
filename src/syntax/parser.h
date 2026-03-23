#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <iostream>
#include <fstream>
#include "../lexical/token.h"
#include "../AST/ast.h"
#include "../AST/stmt.h"
#include "../AST/expr.h"
#include "../../symbol_table.h"

class Parser {
private:
    vector<Token> tokens;
    SymbolTable& symTable;
    int current = 0;

    // Logic helpers
    Token advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    bool check(TokenType type);
    bool match(TokenType type);
    Token consume(TokenType type, string msg);
    void error(Token token, string msg);

    // Grammar functions - Support for more types
    Stmt* declaration();
    Stmt* statement();
    Stmt* assignment();
    
    // Expression levels for math logic
    Expr* expression();
    Expr* term();
    Expr* factor();
    Expr* primary();

public:
    Parser(vector<Token> tokens,SymbolTable& st);
    vector<Stmt*> parse();
};

#endif