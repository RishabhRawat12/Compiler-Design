#ifndef LEXER_H
#define LEXER_H

#include<iostream>
#include<string>
#include<vector>
#include "token.h"

using namespace std;

class Lexer{
    private:
        string source;
        vector<Token> tokens;

        //helper pointers for scanning
        size_t start;
        size_t current;
        int line,col;

        //helper functiond for scanning 
        bool isAtend();
        char advance();
        char peek();
        char peekNext();

        //core functions
        void scanToken();
        void identifier();
        void number();
        void stringLiteral();

        //to add tkens that are being identified
        void addToken(TokenType type);

    public:
    //construcotr having read the source code
        Lexer(const string& src);

        //tokenizer
        vector<Token> tokenize();
};

#endif