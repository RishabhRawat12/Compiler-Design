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
        int start;
        int current;
        int line;

        //helper functiond for scanning 
        bool isAtend();
        char advance();
        char peek();

        //core functions
        void scanToken();
        void identifier();
        void number();

        //to add tkens that are being identified
        void addToke(TokenType type);

    public:
    //construcotr having read the source code
        Lexer(string src);

        //tokenizer
        vector<Token> tokenize();
};

#endif
