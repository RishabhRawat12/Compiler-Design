#pragma once
#include <string>
#include <vector>

enum class TokenType {
    // Keywords
    KW_INT, KW_FLOAT, KW_CHAR, KW_VOID, KW_IF, KW_ELSE, KW_WHILE, KW_FOR,
    KW_RETURN, KW_BREAK, KW_CONTINUE, KW_DO, KW_STRUCT, KW_TYPEDEF,
    // Identifiers and literals
    IDENTIFIER, INT_LITERAL, FLOAT_LITERAL, CHAR_LITERAL, STRING_LITERAL,
    // Operators
    OP_PLUS, OP_MINUS, OP_STAR, OP_SLASH, OP_PERCENT,
    OP_ASSIGN, OP_PLUS_ASSIGN, OP_MINUS_ASSIGN, OP_STAR_ASSIGN, OP_SLASH_ASSIGN,
    OP_EQ, OP_NEQ, OP_LT, OP_LE, OP_GT, OP_GE,
    OP_AND, OP_OR, OP_NOT,
    OP_BIT_AND, OP_BIT_OR, OP_BIT_XOR, OP_BIT_NOT, OP_LSHIFT, OP_RSHIFT,
    OP_INC, OP_DEC, OP_ARROW, OP_DOT,
    // Delimiters
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    SEMICOLON, COMMA, COLON, QUESTION,
    // Special
    END_OF_FILE,
    // Error token
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    std::string typeName() const;
};

struct LexicalError {
    std::string message;
    int line;
    int column;
};

struct LexerResult {
    std::vector<Token> tokens;
    std::vector<LexicalError> errors;
};

LexerResult tokenize(const std::string& source);
std::string tokenTypeToString(TokenType t);
