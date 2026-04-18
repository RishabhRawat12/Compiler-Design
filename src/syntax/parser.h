#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include "../lexical/token.h"
#include "../AST/stmt.h"
#include "../AST/expr.h"
#include "../../symbol_table.h"

using namespace std;

struct ParseError {
    int line, col;
    string message;
    ParseError(int l, int c, string m) : line(l), col(c), message(m) {}
};

class Parser {
    vector<Token> tokens;
    int current;
    SymbolTable& symTable;
    vector<ParseError> errors;
    bool panicMode = false;

    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(TokenType type);
    Token consume(TokenType type, string msg);
    void error(Token token, string msg);
    void synchronize();

    unique_ptr<Stmt> declaration();
    unique_ptr<Stmt> statement();
    unique_ptr<Stmt> ifStatement();
    unique_ptr<Stmt> forStatement();
    unique_ptr<Stmt> assignment();
    vector<unique_ptr<Stmt>> block();

    unique_ptr<Expr> expression();
    unique_ptr<Expr> comparison();
    unique_ptr<Expr> term();
    unique_ptr<Expr> factor();
    unique_ptr<Expr> primary();

public:
    Parser(vector<Token> tokens, SymbolTable& st);
    vector<unique_ptr<Stmt>> parse();
    vector<ParseError> getErrors();
};

#endif