#include "lexical_analyzer.h"
#include <cctype>
#include <unordered_map>

static const std::unordered_map<std::string, TokenType> keywords = {
    {"int", TokenType::KW_INT}, {"float", TokenType::KW_FLOAT},
    {"char", TokenType::KW_CHAR}, {"void", TokenType::KW_VOID},
    {"if", TokenType::KW_IF}, {"else", TokenType::KW_ELSE},
    {"while", TokenType::KW_WHILE}, {"for", TokenType::KW_FOR},
    {"return", TokenType::KW_RETURN}, {"break", TokenType::KW_BREAK},
    {"continue", TokenType::KW_CONTINUE}, {"do", TokenType::KW_DO},
    {"struct", TokenType::KW_STRUCT}, {"typedef", TokenType::KW_TYPEDEF}
};

std::string tokenTypeToString(TokenType t) {
    switch (t) {
        case TokenType::KW_INT: return "KW_INT";
        case TokenType::KW_FLOAT: return "KW_FLOAT";
        case TokenType::KW_CHAR: return "KW_CHAR";
        case TokenType::KW_VOID: return "KW_VOID";
        case TokenType::KW_IF: return "KW_IF";
        case TokenType::KW_ELSE: return "KW_ELSE";
        case TokenType::KW_WHILE: return "KW_WHILE";
        case TokenType::KW_FOR: return "KW_FOR";
        case TokenType::KW_RETURN: return "KW_RETURN";
        case TokenType::KW_BREAK: return "KW_BREAK";
        case TokenType::KW_CONTINUE: return "KW_CONTINUE";
        case TokenType::KW_DO: return "KW_DO";
        case TokenType::KW_STRUCT: return "KW_STRUCT";
        case TokenType::KW_TYPEDEF: return "KW_TYPEDEF";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INT_LITERAL: return "INT_LITERAL";
        case TokenType::FLOAT_LITERAL: return "FLOAT_LITERAL";
        case TokenType::CHAR_LITERAL: return "CHAR_LITERAL";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::OP_PLUS: return "OP_PLUS";
        case TokenType::OP_MINUS: return "OP_MINUS";
        case TokenType::OP_STAR: return "OP_STAR";
        case TokenType::OP_SLASH: return "OP_SLASH";
        case TokenType::OP_PERCENT: return "OP_PERCENT";
        case TokenType::OP_ASSIGN: return "OP_ASSIGN";
        case TokenType::OP_PLUS_ASSIGN: return "OP_PLUS_ASSIGN";
        case TokenType::OP_MINUS_ASSIGN: return "OP_MINUS_ASSIGN";
        case TokenType::OP_STAR_ASSIGN: return "OP_STAR_ASSIGN";
        case TokenType::OP_SLASH_ASSIGN: return "OP_SLASH_ASSIGN";
        case TokenType::OP_EQ: return "OP_EQ";
        case TokenType::OP_NEQ: return "OP_NEQ";
        case TokenType::OP_LT: return "OP_LT";
        case TokenType::OP_LE: return "OP_LE";
        case TokenType::OP_GT: return "OP_GT";
        case TokenType::OP_GE: return "OP_GE";
        case TokenType::OP_AND: return "OP_AND";
        case TokenType::OP_OR: return "OP_OR";
        case TokenType::OP_NOT: return "OP_NOT";
        case TokenType::OP_BIT_AND: return "OP_BIT_AND";
        case TokenType::OP_BIT_OR: return "OP_BIT_OR";
        case TokenType::OP_BIT_XOR: return "OP_BIT_XOR";
        case TokenType::OP_BIT_NOT: return "OP_BIT_NOT";
        case TokenType::OP_LSHIFT: return "OP_LSHIFT";
        case TokenType::OP_RSHIFT: return "OP_RSHIFT";
        case TokenType::OP_INC: return "OP_INC";
        case TokenType::OP_DEC: return "OP_DEC";
        case TokenType::OP_ARROW: return "OP_ARROW";
        case TokenType::OP_DOT: return "OP_DOT";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::COLON: return "COLON";
        case TokenType::QUESTION: return "QUESTION";
        case TokenType::END_OF_FILE: return "EOF";
        default: return "UNKNOWN";
    }
}

std::string Token::typeName() const {
    return tokenTypeToString(type);
}

struct Lexer {
    const std::string& src;
    size_t pos;
    int line;
    int col;
    LexerResult result;

    Lexer(const std::string& s) : src(s), pos(0), line(1), col(1) {}

    char current() { return pos < src.size() ? src[pos] : '\0'; }
    char peek(int offset = 1) { return (pos + offset) < src.size() ? src[pos + offset] : '\0'; }

    void advance() {
        if (pos < src.size()) {
            if (src[pos] == '\n') { line++; col = 1; }
            else { col++; }
            pos++;
        }
    }

    void skipWhitespace() {
        while (pos < src.size() && std::isspace(current())) advance();
    }

    void skipLineComment() {
        while (pos < src.size() && current() != '\n') advance();
    }

    void skipBlockComment() {
        int startLine = line, startCol = col;
        advance(); advance(); // skip /*
        while (pos < src.size()) {
            if (current() == '*' && peek() == '/') {
                advance(); advance();
                return;
            }
            advance();
        }
        result.errors.push_back({"Unterminated block comment", startLine, startCol});
    }

    Token readIdentifierOrKeyword() {
        int startLine = line, startCol = col;
        std::string val;
        while (pos < src.size() && (std::isalnum(current()) || current() == '_')) {
            val += current(); advance();
        }
        auto it = keywords.find(val);
        TokenType t = (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
        return {t, val, startLine, startCol};
    }

    Token readNumber() {
        int startLine = line, startCol = col;
        std::string val;
        bool isFloat = false;
        while (pos < src.size() && std::isdigit(current())) {
            val += current(); advance();
        }
        if (current() == '.' && std::isdigit(peek())) {
            isFloat = true;
            val += current(); advance();
            while (pos < src.size() && std::isdigit(current())) {
                val += current(); advance();
            }
        }
        if (current() == 'e' || current() == 'E') {
            isFloat = true;
            val += current(); advance();
            if (current() == '+' || current() == '-') { val += current(); advance(); }
            while (pos < src.size() && std::isdigit(current())) {
                val += current(); advance();
            }
        }
        return {isFloat ? TokenType::FLOAT_LITERAL : TokenType::INT_LITERAL, val, startLine, startCol};
    }

    Token readCharLiteral() {
        int startLine = line, startCol = col;
        std::string val = "'";
        advance(); // skip '
        if (current() == '\\') { val += '\\'; advance(); }
        if (pos < src.size()) { val += current(); advance(); }
        if (current() == '\'') { val += '\''; advance(); }
        else { result.errors.push_back({"Unterminated char literal", startLine, startCol}); }
        return {TokenType::CHAR_LITERAL, val, startLine, startCol};
    }

    Token readStringLiteral() {
        int startLine = line, startCol = col;
        std::string val = "\"";
        advance(); // skip "
        while (pos < src.size() && current() != '"' && current() != '\n') {
            if (current() == '\\') { val += '\\'; advance(); }
            val += current(); advance();
        }
        if (current() == '"') { val += '"'; advance(); }
        else { result.errors.push_back({"Unterminated string literal", startLine, startCol}); }
        return {TokenType::STRING_LITERAL, val, startLine, startCol};
    }

    LexerResult run() {
        while (pos < src.size()) {
            skipWhitespace();
            if (pos >= src.size()) break;

            int startLine = line, startCol = col;
            char c = current();

            // Comments
            if (c == '/' && peek() == '/') { skipLineComment(); continue; }
            if (c == '/' && peek() == '*') { skipBlockComment(); continue; }

            // Identifier or keyword
            if (std::isalpha(c) || c == '_') { result.tokens.push_back(readIdentifierOrKeyword()); continue; }

            // Numbers
            if (std::isdigit(c)) { result.tokens.push_back(readNumber()); continue; }

            // Char literal
            if (c == '\'') { result.tokens.push_back(readCharLiteral()); continue; }

            // String literal
            if (c == '"') { result.tokens.push_back(readStringLiteral()); continue; }

            // Operators and punctuation
            auto emit = [&](TokenType t, const std::string& v, int n = 1) {
                result.tokens.push_back({t, v, startLine, startCol});
                for (int i = 0; i < n; i++) advance();
            };

            switch (c) {
                case '+':
                    if (peek() == '+') emit(TokenType::OP_INC, "++", 2);
                    else if (peek() == '=') emit(TokenType::OP_PLUS_ASSIGN, "+=", 2);
                    else emit(TokenType::OP_PLUS, "+");
                    break;
                case '-':
                    if (peek() == '-') emit(TokenType::OP_DEC, "--", 2);
                    else if (peek() == '=') emit(TokenType::OP_MINUS_ASSIGN, "-=", 2);
                    else if (peek() == '>') emit(TokenType::OP_ARROW, "->", 2);
                    else emit(TokenType::OP_MINUS, "-");
                    break;
                case '*':
                    if (peek() == '=') emit(TokenType::OP_STAR_ASSIGN, "*=", 2);
                    else emit(TokenType::OP_STAR, "*");
                    break;
                case '/':
                    if (peek() == '=') emit(TokenType::OP_SLASH_ASSIGN, "/=", 2);
                    else emit(TokenType::OP_SLASH, "/");
                    break;
                case '%': emit(TokenType::OP_PERCENT, "%"); break;
                case '=':
                    if (peek() == '=') emit(TokenType::OP_EQ, "==", 2);
                    else emit(TokenType::OP_ASSIGN, "=");
                    break;
                case '!':
                    if (peek() == '=') emit(TokenType::OP_NEQ, "!=", 2);
                    else emit(TokenType::OP_NOT, "!");
                    break;
                case '<':
                    if (peek() == '=') emit(TokenType::OP_LE, "<=", 2);
                    else if (peek() == '<') emit(TokenType::OP_LSHIFT, "<<", 2);
                    else emit(TokenType::OP_LT, "<");
                    break;
                case '>':
                    if (peek() == '=') emit(TokenType::OP_GE, ">=", 2);
                    else if (peek() == '>') emit(TokenType::OP_RSHIFT, ">>", 2);
                    else emit(TokenType::OP_GT, ">");
                    break;
                case '&':
                    if (peek() == '&') emit(TokenType::OP_AND, "&&", 2);
                    else emit(TokenType::OP_BIT_AND, "&");
                    break;
                case '|':
                    if (peek() == '|') emit(TokenType::OP_OR, "||", 2);
                    else emit(TokenType::OP_BIT_OR, "|");
                    break;
                case '^': emit(TokenType::OP_BIT_XOR, "^"); break;
                case '~': emit(TokenType::OP_BIT_NOT, "~"); break;
                case '.': emit(TokenType::OP_DOT, "."); break;
                case '(': emit(TokenType::LPAREN, "("); break;
                case ')': emit(TokenType::RPAREN, ")"); break;
                case '{': emit(TokenType::LBRACE, "{"); break;
                case '}': emit(TokenType::RBRACE, "}"); break;
                case '[': emit(TokenType::LBRACKET, "["); break;
                case ']': emit(TokenType::RBRACKET, "]"); break;
                case ';': emit(TokenType::SEMICOLON, ";"); break;
                case ',': emit(TokenType::COMMA, ","); break;
                case ':': emit(TokenType::COLON, ":"); break;
                case '?': emit(TokenType::QUESTION, "?"); break;
                default:
                    result.errors.push_back({"Unexpected character: '" + std::string(1, c) + "'", startLine, startCol});
                    advance();
                    break;
            }
        }
        result.tokens.push_back({TokenType::END_OF_FILE, "", line, col});
        return result;
    }
};

LexerResult tokenize(const std::string& source) {
    Lexer lexer(source);
    return lexer.run();
}
