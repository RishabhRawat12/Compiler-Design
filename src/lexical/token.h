#ifndef TOKEN_H
#define TOKEN_H

#include<iostream>
#include<string>

using namespace std;

enum TokenType{
    TOK_KEYWORD,
    TOK_IDENTIFIER,
    TOK_NUM_LITERAL,
    TOK_OPERATOR,
    TOK_PUNCTUATION,
    TOK_EOF,
    TOK_UNKNOWN
};
//token structure defination
struct Token
{
    TokenType type;
    string lexeme;
    int line;
    Token(){//default constructor
        type=TOK_UNKNOWN;
        lexeme="";
        line=-1;
    }

    Token(TokenType t,string lex,int l){//parameterized constructor
        type=t;
        lexeme=lex;
        line=l;
    }
    //priting token details
    void printTokenInfo(){
        cout<<"[Line "<<line<<"] Type : "<<type<<" | Lexeme: "<<lexeme<<endl;
    }
};


#endif 
