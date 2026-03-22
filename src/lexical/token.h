#ifndef TOKEN_H
#define TOKEN_H

#include<iostream>
#include<string>

using namespace std;

enum TokenType{
    // Keywords
    KEYWORD_INT,
    KEYWORD_CHAR,
    KEYWORD_STRING,

    // Identifiers & Literals
    IDENTIFIER,
    NUMBER,
    STRING_LITERAL,

    // Operators (needed specifically for parser later, don't group them)
    PLUS, 
    MINUS, 
    MULTIPLY, 
    DIVIDE, 
    ASSIGN,

    // Delimiters
    SEMICOLON,

    // Special tokens
    END_OF_FILE,
    UNKNOWN
};


//token structure defination
struct Token
{
    TokenType type;
    string lexeme;
    int line;
    int col;
    Token(){//default constructor
        type=UNKNOWN;
        lexeme="";
        line=-1;
        col=-1;
    }

    Token(TokenType t,string lex,int l,int column){//parameterized constructor
        type=t;
        lexeme=lex;
        line=l;
        col=column;
    }

    string getTypeName(){
        switch(type){
            case KEYWORD_INT:return "KEYWORD_INT";
            case KEYWORD_CHAR:return "KEYWORD_CHAR";
            case KEYWORD_STRING:return "KEYWORD_STRING";
            case IDENTIFIER:return "IDENTIFIER";
            case NUMBER:return "NUMBER";
            case STRING_LITERAL:return "STRING_LITERAL";
            case PLUS:return "PLUS";
            case MINUS:return "MINUS";
            case MULTIPLY:return "MULTIPLY";
            case DIVIDE:return "DIVIDE";
            case ASSIGN:return "ASSIGN";
            case SEMICOLON:return "SEMICOLON";
            case END_OF_FILE:return "END_OF_FILE";
            default: return "UNKNOWN";
        }
    }

    //priting token details
    void printTokenInfo(){
        cout<<"[Line "<<line<<"] Type : "<<type<<" | Lexeme: "<<lexeme<<endl;
    }
};


#endif 