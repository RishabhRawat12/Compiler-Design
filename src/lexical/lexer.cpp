#include "lexer.h"
#include <cctype>

Lexer::Lexer(string src) {
    source = src;
    start = 0; current = 0; line = 1; col = 1;
}

bool Lexer::isAtEnd() { return current >= source.length(); }

char Lexer::advance() { col++; return source[current++]; }

char Lexer::peek() { if (isAtEnd()) return '\0'; return source[current]; }

char Lexer::peekNext() { if (current + 1 >= source.length()) return '\0'; return source[current + 1]; }

void Lexer::addError(string msg) { errors.push_back(LexError(line, col, msg)); }

void Lexer::addToken(TokenType type) {
    string_view text(source.c_str() + start, current - start);
    int tokenCol = col - (current - start);
    tokens.push_back(Token(type, string(text), line, tokenCol));
}

vector<Token> Lexer::tokenize() {
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }
    tokens.push_back(Token(END_OF_FILE, "", line, col));
    return tokens;
}

vector<LexError> Lexer::getErrors() { return errors; }

void Lexer::scanToken() {
    char c = advance();
    switch(c) {
        case '(': addToken(LEFT_PAREN); break;
        case ')': addToken(RIGHT_PAREN); break;
        case '{': addToken(LEFT_BRACE); break;
        case '}': addToken(RIGHT_BRACE); break;
        case ';': addToken(SEMICOLON); break;
        case ',': addToken(COMMA); break;
        case '+': addToken(PLUS); break;
        case '-': addToken(MINUS); break;
        case '*': addToken(MULTIPLY); break;
        case '/':
            if (peek() == '/') while (peek() != '\n' && !isAtEnd()) advance();
            else if (peek() == '*') {
                advance();
                while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                    if (peek() == '\n') { line++; col = 1; }
                    advance();
                }
                advance(); advance();
            } else addToken(DIVIDE);
            break;
        case '=': if (peek() == '=') { advance(); addToken(EQUAL_EQUAL); } else addToken(ASSIGN); break;
        case '>': if (peek() == '=') { advance(); addToken(GREATER_EQUAL); } else addToken(GREATER); break;
        case '<': if (peek() == '=') { advance(); addToken(LESS_EQUAL); } else addToken(LESS); break;
        case '!': if (peek() == '=') { advance(); addToken(NOT_EQUAL); } else addToken(BANG); break;
        case '"': stringLiteral(); break;
        case '\'': charLiteral(); break;
        case ' ': case '\r': case '\t': break;
        case '\n': line++; col = 1; break;
        default:
            if (isdigit(c)) number();
            else if (isalpha(c) || c == '_') identifier();
            else { addError("Invalid character"); addToken(UNKNOWN); }
    }
}

void Lexer::number() {
    while (isdigit(peek())) advance();
    addToken(NUMBER);
}

void Lexer::identifier() {
    while (isalnum(peek()) || peek() == '_') advance();
    string text = source.substr(start, current - start);
    
    if (text == "int") addToken(KEYWORD_INT);
    else if (text == "char") addToken(KEYWORD_CHAR);
    else if (text == "string") addToken(KEYWORD_STRING);
    else if (text == "void") addToken(KEYWORD_VOID);
    else if (text == "if") addToken(KEYWORD_IF);
    else if (text == "else") addToken(KEYWORD_ELSE);
    else if (text == "for") addToken(KEYWORD_FOR);
    else if (text == "return") addToken(KEYWORD_RETURN);
    else if (text == "break") addToken(KEYWORD_BREAK);
    else if (text == "continue") addToken(KEYWORD_CONTINUE);
    else addToken(IDENTIFIER);
}

void Lexer::stringLiteral() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') { line++; col = 1; }
        advance();
    }
    if (isAtEnd()) { addError("Unterminated string"); return; }
    advance();
    addToken(STRING_LITERAL);
}

void Lexer::charLiteral() {
    if (peek() == '\0') { addError("Invalid char literal"); return; }
    advance();
    if (peek() != '\'') { addError("Invalid char format"); return; }
    advance();
    addToken(CHAR_LITERAL);
}
