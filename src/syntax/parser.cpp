#include "parser.h"

Parser::Parser(vector<Token> tokens, SymbolTable& st) : symTable(st) {
    this->tokens = tokens;
    this->current = 0;
}

vector<unique_ptr<Stmt>> Parser::parse() {
    vector<unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        if (match(SEMICOLON)) continue;
        auto stmt = declaration();
        if (stmt) statements.push_back(move(stmt));
    }
    return statements;
}

void Parser::synchronize() {
    panicMode = false;
    advance(); 
    
    while (!isAtEnd()) {
        if (previous().type == SEMICOLON) return;
        switch (peek().type) {
            case KEYWORD_IF: case KEYWORD_FOR: case KEYWORD_RETURN:
            case KEYWORD_INT: case KEYWORD_CHAR: case KEYWORD_STRING: case KEYWORD_VOID:
                return;
            default: advance();
        }
    }
}

unique_ptr<Stmt> Parser::declaration() {
    try {
        if (match(KEYWORD_INT) || match(KEYWORD_CHAR) || match(KEYWORD_STRING) || match(KEYWORD_VOID)) {
            Token typeToken = previous();
            string semanticType = "unknown";
            if (typeToken.type == KEYWORD_INT) semanticType = "int";
            else if (typeToken.type == KEYWORD_CHAR) semanticType = "char";
            else if (typeToken.type == KEYWORD_STRING) semanticType = "string";
            else if (typeToken.type == KEYWORD_VOID) semanticType = "void";

            Token name = consume(IDENTIFIER, "Expect variable/function name");

            if (match(LEFT_PAREN)) {
                vector<pair<string, string>> params;
                if (!check(RIGHT_PAREN)) {
                    do {
                        Token paramTok = advance();
                        string pType = "unknown";
                        if (paramTok.type == KEYWORD_INT) pType = "int";
                        else if (paramTok.type == KEYWORD_CHAR) pType = "char";
                        else if (paramTok.type == KEYWORD_STRING) pType = "string";
                        else error(paramTok, "Expected parameter type");

                        Token pName = consume(IDENTIFIER, "Expect parameter name");
                        params.push_back({pType, pName.lexeme});
                    } while (match(COMMA));
                }
                consume(RIGHT_PAREN, "Expect ')' after parameters");
                consume(LEFT_BRACE, "Expect '{' before function body");
                auto body = block();
                return make_unique<FunctionStmt>(semanticType, name.lexeme, params, move(body), name.line);
            } else {
                if (semanticType == "void") error(typeToken, "Variable cannot have 'void' type");
                consume(ASSIGN, "Missing '='");
                auto val = expression();
                consume(SEMICOLON, "Missing ';'");
                return make_unique<DeclarationStmt>(semanticType, name.lexeme, move(val), name.line);
            }
        }
        return statement();
    } catch (...) { synchronize(); return nullptr; }
}

unique_ptr<Stmt> Parser::statement() {
    if (match(KEYWORD_IF)) return ifStatement();
    if (match(KEYWORD_FOR)) return forStatement();
    if (match(KEYWORD_RETURN)) {
        Token retToken = previous();
        unique_ptr<Expr> val = nullptr;
        if (!check(SEMICOLON)) val = expression();
        consume(SEMICOLON, "Expect ';' after return value");
        return make_unique<ReturnStmt>(move(val), retToken.line);
    }
    if (match(KEYWORD_BREAK)) {
        Token tok = previous();
        consume(SEMICOLON, "Expect ';' after break");
        return make_unique<BreakStmt>(tok.line);
    }
    if (match(KEYWORD_CONTINUE)) {
        Token tok = previous();
        consume(SEMICOLON, "Expect ';' after continue");
        return make_unique<ContinueStmt>(tok.line);
    }
    return assignment();
}

unique_ptr<Stmt> Parser::ifStatement() {
    Token ifToken = previous();
    consume(LEFT_PAREN, "Missing '(' after if");
    auto condition = expression();
    consume(RIGHT_PAREN, "Missing ')'");
    consume(LEFT_BRACE, "Missing '{'");
    auto thenBranch = block();
    vector<unique_ptr<Stmt>> elseBranch;
    if (match(KEYWORD_ELSE)) {
        consume(LEFT_BRACE, "Missing '{' after else");
        elseBranch = block();
    }
    return make_unique<IfStmt>(move(condition), move(thenBranch), move(elseBranch), ifToken.line);
}

unique_ptr<Stmt> Parser::forStatement() {
    Token forToken = previous();
    consume(LEFT_PAREN, "Missing '(' after for");
    
    unique_ptr<Stmt> init = nullptr;

    if (match(KEYWORD_INT) || match(KEYWORD_CHAR) || match(KEYWORD_STRING)) {
        Token typeToken = previous();
        string semanticType = "int";
        if (typeToken.type == KEYWORD_CHAR) semanticType = "char";
        else if (typeToken.type == KEYWORD_STRING) semanticType = "string";

        Token name = consume(IDENTIFIER, "Expect variable name");
        consume(ASSIGN, "Missing '='");
        auto val = expression();
        consume(SEMICOLON, "Missing ';'");
        init = make_unique<DeclarationStmt>(semanticType, name.lexeme, move(val), name.line);
    } else {
        auto initExpr = expression();
        consume(ASSIGN, "Missing '=' in for init");
        Token initEquals = previous();
        auto initVal = expression();
        consume(SEMICOLON, "Missing ';' in for");
        init = make_unique<AssignmentStmt>(move(initExpr), move(initVal), initEquals.line);
    }

    auto condition = expression();
    consume(SEMICOLON, "Missing ';' in for condition");

    auto incExpr = expression();
    unique_ptr<Stmt> increment = nullptr;

    if (match(PLUS)) {
        if (match(PLUS)) {
            VariableExpr* varExpr = dynamic_cast<VariableExpr*>(incExpr.get());
            if (varExpr) {
                auto one = make_unique<NumberExpr>(1, varExpr->line);
                auto add = make_unique<BinaryExpr>(make_unique<VariableExpr>(varExpr->name, varExpr->line), "+", move(one), varExpr->line);
                increment = make_unique<AssignmentStmt>(move(incExpr), move(add), varExpr->line);
            } else {
                error(previous(), "Invalid operand for ++");
            }
        } else {
            error(previous(), "Expected ++");
        }
    } else if (match(MINUS)) {
        if (match(MINUS)) {
            VariableExpr* varExpr = dynamic_cast<VariableExpr*>(incExpr.get());
            if (varExpr) {
                auto one = make_unique<NumberExpr>(1, varExpr->line);
                auto sub = make_unique<BinaryExpr>(make_unique<VariableExpr>(varExpr->name, varExpr->line), "-", move(one), varExpr->line);
                increment = make_unique<AssignmentStmt>(move(incExpr), move(sub), varExpr->line);
            } else {
                error(previous(), "Invalid operand for --");
            }
        } else {
            error(previous(), "Expected --");
        }
    } else {
        consume(ASSIGN, "Missing '=' in for increment");
        Token incEquals = previous();
        auto incVal = expression();
        increment = make_unique<AssignmentStmt>(move(incExpr), move(incVal), incEquals.line);
    }

    consume(RIGHT_PAREN, "Missing ')'");
    consume(LEFT_BRACE, "Missing '{'");
    auto body = block();
    
    return make_unique<ForStmt>(move(init), move(condition), move(increment), move(body), forToken.line);
}

vector<unique_ptr<Stmt>> Parser::block() {
    vector<unique_ptr<Stmt>> statements;
    while (!check(RIGHT_BRACE) && !isAtEnd()) {
        auto stmt = declaration();
        if (stmt) statements.push_back(move(stmt));
    }
    consume(RIGHT_BRACE, "Missing '}'");
    return statements;
}

unique_ptr<Stmt> Parser::assignment() {
    auto expr = expression();

    if (match(PLUS)) {
        if (match(PLUS)) {
            consume(SEMICOLON, "Missing ';' after ++");
            VariableExpr* varExpr = dynamic_cast<VariableExpr*>(expr.get());
            if (varExpr) {
                auto one = make_unique<NumberExpr>(1, varExpr->line);
                auto add = make_unique<BinaryExpr>(make_unique<VariableExpr>(varExpr->name, varExpr->line), "+", move(one), varExpr->line);
                return make_unique<AssignmentStmt>(move(expr), move(add), varExpr->line);
            }
            error(previous(), "Invalid operand for ++");
            synchronize();
            return nullptr;
        }
    } else if (match(MINUS)) {
        if (match(MINUS)) {
            consume(SEMICOLON, "Missing ';' after --");
            VariableExpr* varExpr = dynamic_cast<VariableExpr*>(expr.get());
            if (varExpr) {
                auto one = make_unique<NumberExpr>(1, varExpr->line);
                auto sub = make_unique<BinaryExpr>(make_unique<VariableExpr>(varExpr->name, varExpr->line), "-", move(one), varExpr->line);
                return make_unique<AssignmentStmt>(move(expr), move(sub), varExpr->line);
            }
            error(previous(), "Invalid operand for --");
            synchronize();
            return nullptr;
        }
    }

    if (match(ASSIGN)) {
        Token equals = previous();
        auto value = expression();
        consume(SEMICOLON, "Missing ';'");
        return make_unique<AssignmentStmt>(move(expr), move(value), equals.line);
    }

    if (match(SEMICOLON)) {
        return nullptr;
    }

    error(peek(), "Expected assignment statement");
    synchronize();
    return nullptr;
}

unique_ptr<Expr> Parser::expression() { return comparison(); }

unique_ptr<Expr> Parser::comparison() {
    auto expr = term();
    while (match(GREATER) || match(LESS) || match(GREATER_EQUAL) || match(LESS_EQUAL) || match(EQUAL_EQUAL) || match(NOT_EQUAL)) {
        Token opToken = previous();
        auto right = term();
        expr = make_unique<BinaryExpr>(move(expr), opToken.lexeme, move(right), opToken.line);
    }
    return expr;
}

unique_ptr<Expr> Parser::term() {
    auto expr = factor();
    while (true) {
        // Lookahead to make sure we don't accidentally eat the first + of a ++
        bool isPlusPlus = check(PLUS) && (current + 1 < tokens.size() && tokens[current + 1].type == PLUS);
        bool isMinusMinus = check(MINUS) && (current + 1 < tokens.size() && tokens[current + 1].type == MINUS);

        if (check(PLUS) && !isPlusPlus) {
            match(PLUS);
            Token opToken = previous();
            auto right = factor();
            expr = make_unique<BinaryExpr>(move(expr), opToken.lexeme, move(right), opToken.line);
        } else if (check(MINUS) && !isMinusMinus) {
            match(MINUS);
            Token opToken = previous();
            auto right = factor();
            expr = make_unique<BinaryExpr>(move(expr), opToken.lexeme, move(right), opToken.line);
        } else {
            break;
        }
    }
    return expr;
}

unique_ptr<Expr> Parser::factor() {
    auto expr = primary();
    while (match(MULTIPLY) || match(DIVIDE)) {
        Token opToken = previous();
        auto right = primary();
        expr = make_unique<BinaryExpr>(move(expr), opToken.lexeme, move(right), opToken.line);
    }
    return expr;
}

unique_ptr<Expr> Parser::primary() {
    if (match(NUMBER)) return make_unique<NumberExpr>(stoi(previous().lexeme), previous().line);
    if (match(STRING_LITERAL)) return make_unique<StringExpr>(previous().lexeme, previous().line);
    if (match(CHAR_LITERAL)) return make_unique<CharExpr>(previous().lexeme, previous().line);
    
    if (match(IDENTIFIER)) {
        Token var = previous();
        if (match(LEFT_PAREN)) {
            vector<unique_ptr<Expr>> args;
            if (!check(RIGHT_PAREN)) {
                do { args.push_back(expression()); } while (match(COMMA));
            }
            consume(RIGHT_PAREN, "Expect ')' after arguments");
            return make_unique<CallExpr>(var.lexeme, move(args), var.line);
        }
        return make_unique<VariableExpr>(var.lexeme, var.line);
    }
    error(peek(), "Invalid expression");
    return nullptr;
}

Token Parser::advance() { if (!isAtEnd()) current++; return previous(); }
bool Parser::isAtEnd() { return peek().type == END_OF_FILE; }
Token Parser::peek() { return tokens[current]; }
Token Parser::previous() { return tokens[current - 1]; }
bool Parser::check(TokenType type) { if (isAtEnd()) return false; return peek().type == type; }
bool Parser::match(TokenType type) { if (check(type)) { advance(); return true; } return false; }

Token Parser::consume(TokenType type, string msg) {
    if (check(type)) return advance();
    error(peek(), msg);
    throw runtime_error("Parse error");
}

void Parser::error(Token token, string msg) {
    if (panicMode) return;
    panicMode = true;
    errors.push_back(ParseError(token.line, token.col, msg));
}

vector<ParseError> Parser::getErrors() { return errors; }