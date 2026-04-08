#pragma once
#include <string>
#include <vector>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

struct LexError {
    int line;
    int col;
    string message;

    LexError(int l, int c, const string& msg) : line(l), col(c), message(msg) {}
    json toJson() const { return { {"line", line}, {"col", col}, {"message", message} }; }
};

enum TokenType {
    KEYWORD_INT, KEYWORD_CHAR, KEYWORD_STRING, KEYWORD_VOID,
    KEYWORD_IF, KEYWORD_ELSE, KEYWORD_FOR,
    KEYWORD_RETURN, KEYWORD_BREAK, KEYWORD_CONTINUE,
    IDENTIFIER, NUMBER, STRING_LITERAL, CHAR_LITERAL,
    PLUS, MINUS, MULTIPLY, DIVIDE, ASSIGN,
    EQUAL_EQUAL, NOT_EQUAL, BANG, GREATER, LESS, GREATER_EQUAL, LESS_EQUAL,
    SEMICOLON, COMMA, LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    END_OF_FILE, UNKNOWN
};

class Token {
public:
    TokenType type;
    string lexeme;
    int line;
    int col;

    Token() : type(UNKNOWN), lexeme(""), line(-1), col(-1) {}
    Token(TokenType t, string lex, int l, int column) : type(t), lexeme(lex), line(l), col(column) {}

    string getTypeName() const {
        switch (type) {
            case KEYWORD_INT: return "KEYWORD_INT";
            case KEYWORD_CHAR: return "KEYWORD_CHAR";
            case KEYWORD_STRING: return "KEYWORD_STRING";
            case KEYWORD_VOID: return "KEYWORD_VOID";
            case KEYWORD_IF: return "KEYWORD_IF";
            case KEYWORD_ELSE: return "KEYWORD_ELSE";
            case KEYWORD_FOR: return "KEYWORD_FOR";
            case KEYWORD_RETURN: return "KEYWORD_RETURN";
            case KEYWORD_BREAK: return "KEYWORD_BREAK";
            case KEYWORD_CONTINUE: return "KEYWORD_CONTINUE";
            case IDENTIFIER: return "IDENTIFIER";
            case NUMBER: return "NUMBER";
            case STRING_LITERAL: return "STRING_LITERAL";
            case CHAR_LITERAL: return "CHAR_LITERAL";
            case PLUS: return "PLUS";
            case MINUS: return "MINUS";
            case MULTIPLY: return "MULTIPLY";
            case DIVIDE: return "DIVIDE";
            case ASSIGN: return "ASSIGN";
            case EQUAL_EQUAL: return "EQUAL_EQUAL";
            case NOT_EQUAL: return "NOT_EQUAL";
            case BANG: return "BANG";
            case GREATER: return "GREATER";
            case LESS: return "LESS";
            case GREATER_EQUAL: return "GREATER_EQUAL";
            case LESS_EQUAL: return "LESS_EQUAL";
            case SEMICOLON: return "SEMICOLON";
            case COMMA: return "COMMA";
            case LEFT_PAREN: return "LEFT_PAREN";
            case RIGHT_PAREN: return "RIGHT_PAREN";
            case LEFT_BRACE: return "LEFT_BRACE";
            case RIGHT_BRACE: return "RIGHT_BRACE";
            case END_OF_FILE: return "END_OF_FILE";
            default: return "UNKNOWN";
        }
    }
    json toJson() const { return { {"type", getTypeName()}, {"lexeme", lexeme}, {"line", line}, {"col", col} }; }
};
