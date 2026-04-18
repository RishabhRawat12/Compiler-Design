#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <string>
#include <vector>
#include "token.h"

using namespace std;

class Lexer {
private:
    string source;
    vector<Token> tokens;

    // Error storage (no more cerr side-effects)
    vector<LexError> errors;

    // helper pointers for scanning
    size_t start;
    size_t current;
    int line, col;

    // helper functions for scanning
    bool isAtEnd();
    char advance();
    char peek();
    char peekNext();

    // core functions
    void scanToken();
    void identifier();
    void number();
    void stringLiteral();
    void charLiteral();

    // to add tokens that are being identified
    void addToken(TokenType type);

public:
    // constructor (removed SymbolTable dependency)
    Lexer(string src);

    // tokenizer
    vector<Token> tokenize();

    // error handling API
    void addError(string msg);
    vector<LexError> getErrors();
};

#endif