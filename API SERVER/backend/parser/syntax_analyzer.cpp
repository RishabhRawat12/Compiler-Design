#include "syntax_analyzer.h"
#include <sstream>
#include <stdexcept>

struct Parser {
    const std::vector<Token>& tokens;
    size_t pos;
    std::vector<SyntaxError> errors;
    bool panicMode = false;

    Parser(const std::vector<Token>& toks) : tokens(toks), pos(0) {}

    const Token& current() { return tokens[pos < tokens.size() ? pos : tokens.size() - 1]; }
    const Token& peek(int offset = 1) {
        size_t idx = pos + offset;
        return tokens[idx < tokens.size() ? idx : tokens.size() - 1];
    }

    Token consume() {
        Token t = current();
        if (t.type != TokenType::END_OF_FILE) pos++;
        return t;
    }

    bool check(TokenType t) { return current().type == t; }

    bool match(TokenType t) {
        if (check(t)) { consume(); return true; }
        return false;
    }

    Token expect(TokenType t, const std::string& msg) {
        if (check(t)) return consume();
        errors.push_back({msg + " (got '" + current().value + "')", current().line, current().column});
        return current();
    }

    bool isTypeName() {
        return check(TokenType::KW_INT) || check(TokenType::KW_FLOAT) ||
               check(TokenType::KW_CHAR) || check(TokenType::KW_VOID);
    }

    std::shared_ptr<ASTNode> makeNode(const std::string& type, const std::string& value = "", int line = 0, int col = 0) {
        auto n = std::make_shared<ASTNode>();
        n->type = type;
        n->value = value;
        n->line = line;
        n->column = col;
        return n;
    }

    // program := (declaration | functionDef)*
    std::shared_ptr<ASTNode> parseProgram() {
        auto node = makeNode("Program", "", 1, 1);
        while (!check(TokenType::END_OF_FILE)) {
            if (isTypeName()) {
                // Look ahead to see if it's a function definition
                size_t savedPos = pos;
                consume(); // type
                bool isFunc = false;
                if (check(TokenType::IDENTIFIER)) {
                    consume();
                    if (check(TokenType::LPAREN)) isFunc = true;
                }
                pos = savedPos;
                if (isFunc) {
                    node->children.push_back(parseFunctionDef());
                } else {
                    node->children.push_back(parseDeclaration());
                }
            } else {
                errors.push_back({"Unexpected token: '" + current().value + "'", current().line, current().column});
                consume();
            }
        }
        return node;
    }

    std::string parseTypeName() {
        Token t = consume();
        std::string typeName = t.value;
        while (check(TokenType::OP_STAR)) { typeName += "*"; consume(); }
        return typeName;
    }

    // functionDef := type identifier '(' paramList ')' block
    std::shared_ptr<ASTNode> parseFunctionDef() {
        auto node = makeNode("FunctionDef", "", current().line, current().column);
        std::string type = parseTypeName();
        node->children.push_back(makeNode("ReturnType", type, current().line, current().column));
        Token name = expect(TokenType::IDENTIFIER, "Expected function name");
        node->value = name.value;
        node->line = name.line;
        node->column = name.column;
        expect(TokenType::LPAREN, "Expected '('");
        node->children.push_back(parseParamList());
        expect(TokenType::RPAREN, "Expected ')'");
        if (check(TokenType::SEMICOLON)) {
            consume();
            node->type = "FunctionDecl";
        } else {
            node->children.push_back(parseBlock());
        }
        return node;
    }

    std::shared_ptr<ASTNode> parseParamList() {
        auto node = makeNode("ParamList");
        if (check(TokenType::RPAREN) || check(TokenType::KW_VOID)) {
            if (check(TokenType::KW_VOID)) consume();
            return node;
        }
        while (isTypeName()) {
            auto param = makeNode("Param", "", current().line, current().column);
            std::string type = parseTypeName();
            param->children.push_back(makeNode("Type", type));
            if (check(TokenType::IDENTIFIER)) {
                Token name = consume();
                param->value = name.value;
            }
            node->children.push_back(param);
            if (!match(TokenType::COMMA)) break;
        }
        return node;
    }

    // declaration := type identifier ('=' expr)? (',' identifier ('=' expr)?)* ';'
    std::shared_ptr<ASTNode> parseDeclaration() {
        auto node = makeNode("Declaration", "", current().line, current().column);
        std::string type = parseTypeName();
        node->children.push_back(makeNode("Type", type));
        do {
            auto varNode = makeNode("VarDecl", "", current().line, current().column);
            Token name = expect(TokenType::IDENTIFIER, "Expected variable name");
            varNode->value = name.value;
            if (match(TokenType::OP_ASSIGN)) {
                varNode->children.push_back(parseExpression());
            }
            node->children.push_back(varNode);
        } while (match(TokenType::COMMA));
        expect(TokenType::SEMICOLON, "Expected ';'");
        return node;
    }

    // block := '{' statement* '}'
    std::shared_ptr<ASTNode> parseBlock() {
        auto node = makeNode("Block", "", current().line, current().column);
        expect(TokenType::LBRACE, "Expected '{'");
        while (!check(TokenType::RBRACE) && !check(TokenType::END_OF_FILE)) {
            node->children.push_back(parseStatement());
        }
        expect(TokenType::RBRACE, "Expected '}'");
        return node;
    }

    std::shared_ptr<ASTNode> parseStatement() {
        if (isTypeName()) return parseDeclaration();
        if (check(TokenType::KW_IF)) return parseIf();
        if (check(TokenType::KW_WHILE)) return parseWhile();
        if (check(TokenType::KW_FOR)) return parseFor();
        if (check(TokenType::KW_DO)) return parseDoWhile();
        if (check(TokenType::KW_RETURN)) return parseReturn();
        if (check(TokenType::KW_BREAK)) { auto n = makeNode("Break","",current().line,current().column); consume(); expect(TokenType::SEMICOLON,"Expected ';'"); return n; }
        if (check(TokenType::KW_CONTINUE)) { auto n = makeNode("Continue","",current().line,current().column); consume(); expect(TokenType::SEMICOLON,"Expected ';'"); return n; }
        if (check(TokenType::LBRACE)) return parseBlock();
        if (check(TokenType::SEMICOLON)) { auto n = makeNode("EmptyStmt","",current().line,current().column); consume(); return n; }
        auto expr = parseExpression();
        expect(TokenType::SEMICOLON, "Expected ';'");
        auto exprStmt = makeNode("ExprStmt", "", expr->line, expr->column);
        exprStmt->children.push_back(expr);
        return exprStmt;
    }

    std::shared_ptr<ASTNode> parseIf() {
        auto node = makeNode("If", "", current().line, current().column);
        consume(); // 'if'
        expect(TokenType::LPAREN, "Expected '('");
        node->children.push_back(parseExpression());
        expect(TokenType::RPAREN, "Expected ')'");
        node->children.push_back(parseStatement());
        if (match(TokenType::KW_ELSE)) {
            node->children.push_back(parseStatement());
        }
        return node;
    }

    std::shared_ptr<ASTNode> parseWhile() {
        auto node = makeNode("While", "", current().line, current().column);
        consume();
        expect(TokenType::LPAREN, "Expected '('");
        node->children.push_back(parseExpression());
        expect(TokenType::RPAREN, "Expected ')'");
        node->children.push_back(parseStatement());
        return node;
    }

    std::shared_ptr<ASTNode> parseFor() {
        auto node = makeNode("For", "", current().line, current().column);
        consume();
        expect(TokenType::LPAREN, "Expected '('");
        if (isTypeName()) node->children.push_back(parseDeclaration());
        else if (!check(TokenType::SEMICOLON)) { node->children.push_back(parseExpression()); expect(TokenType::SEMICOLON,"Expected ';'"); }
        else { node->children.push_back(makeNode("Empty")); consume(); }
        if (!check(TokenType::SEMICOLON)) node->children.push_back(parseExpression());
        else node->children.push_back(makeNode("Empty"));
        expect(TokenType::SEMICOLON, "Expected ';'");
        if (!check(TokenType::RPAREN)) node->children.push_back(parseExpression());
        else node->children.push_back(makeNode("Empty"));
        expect(TokenType::RPAREN, "Expected ')'");
        node->children.push_back(parseStatement());
        return node;
    }

    std::shared_ptr<ASTNode> parseDoWhile() {
        auto node = makeNode("DoWhile", "", current().line, current().column);
        consume();
        node->children.push_back(parseStatement());
        expect(TokenType::KW_WHILE, "Expected 'while'");
        expect(TokenType::LPAREN, "Expected '('");
        node->children.push_back(parseExpression());
        expect(TokenType::RPAREN, "Expected ')'");
        expect(TokenType::SEMICOLON, "Expected ';'");
        return node;
    }

    std::shared_ptr<ASTNode> parseReturn() {
        auto node = makeNode("Return", "", current().line, current().column);
        consume();
        if (!check(TokenType::SEMICOLON)) node->children.push_back(parseExpression());
        expect(TokenType::SEMICOLON, "Expected ';'");
        return node;
    }

    // Expression parsing with precedence climbing
    std::shared_ptr<ASTNode> parseExpression() { return parseAssignment(); }

    std::shared_ptr<ASTNode> parseAssignment() {
        auto left = parseTernary();
        if (check(TokenType::OP_ASSIGN) || check(TokenType::OP_PLUS_ASSIGN) ||
            check(TokenType::OP_MINUS_ASSIGN) || check(TokenType::OP_STAR_ASSIGN) ||
            check(TokenType::OP_SLASH_ASSIGN)) {
            Token op = consume();
            auto node = makeNode("Assign", op.value, op.line, op.column);
            node->children.push_back(left);
            node->children.push_back(parseAssignment());
            return node;
        }
        return left;
    }

    std::shared_ptr<ASTNode> parseTernary() {
        auto cond = parseLogicalOr();
        if (match(TokenType::QUESTION)) {
            auto node = makeNode("Ternary", "", cond->line, cond->column);
            node->children.push_back(cond);
            node->children.push_back(parseExpression());
            expect(TokenType::COLON, "Expected ':'");
            node->children.push_back(parseTernary());
            return node;
        }
        return cond;
    }

    std::shared_ptr<ASTNode> parseLogicalOr() {
        auto left = parseLogicalAnd();
        while (check(TokenType::OP_OR)) {
            Token op = consume();
            auto node = makeNode("BinaryOp", op.value, op.line, op.column);
            node->children.push_back(left);
            node->children.push_back(parseLogicalAnd());
            left = node;
        }
        return left;
    }

    std::shared_ptr<ASTNode> parseLogicalAnd() {
        auto left = parseEquality();
        while (check(TokenType::OP_AND)) {
            Token op = consume();
            auto node = makeNode("BinaryOp", op.value, op.line, op.column);
            node->children.push_back(left);
            node->children.push_back(parseEquality());
            left = node;
        }
        return left;
    }

    std::shared_ptr<ASTNode> parseEquality() {
        auto left = parseRelational();
        while (check(TokenType::OP_EQ) || check(TokenType::OP_NEQ)) {
            Token op = consume();
            auto node = makeNode("BinaryOp", op.value, op.line, op.column);
            node->children.push_back(left);
            node->children.push_back(parseRelational());
            left = node;
        }
        return left;
    }

    std::shared_ptr<ASTNode> parseRelational() {
        auto left = parseAdditive();
        while (check(TokenType::OP_LT) || check(TokenType::OP_LE) ||
               check(TokenType::OP_GT) || check(TokenType::OP_GE)) {
            Token op = consume();
            auto node = makeNode("BinaryOp", op.value, op.line, op.column);
            node->children.push_back(left);
            node->children.push_back(parseAdditive());
            left = node;
        }
        return left;
    }

    std::shared_ptr<ASTNode> parseAdditive() {
        auto left = parseMultiplicative();
        while (check(TokenType::OP_PLUS) || check(TokenType::OP_MINUS)) {
            Token op = consume();
            auto node = makeNode("BinaryOp", op.value, op.line, op.column);
            node->children.push_back(left);
            node->children.push_back(parseMultiplicative());
            left = node;
        }
        return left;
    }

    std::shared_ptr<ASTNode> parseMultiplicative() {
        auto left = parseUnary();
        while (check(TokenType::OP_STAR) || check(TokenType::OP_SLASH) || check(TokenType::OP_PERCENT)) {
            Token op = consume();
            auto node = makeNode("BinaryOp", op.value, op.line, op.column);
            node->children.push_back(left);
            node->children.push_back(parseUnary());
            left = node;
        }
        return left;
    }

    std::shared_ptr<ASTNode> parseUnary() {
        if (check(TokenType::OP_NOT) || check(TokenType::OP_MINUS) || check(TokenType::OP_BIT_NOT) ||
            check(TokenType::OP_INC) || check(TokenType::OP_DEC) || check(TokenType::OP_STAR) || check(TokenType::OP_BIT_AND)) {
            Token op = consume();
            auto node = makeNode("UnaryOp", op.value, op.line, op.column);
            node->children.push_back(parseUnary());
            return node;
        }
        return parsePostfix();
    }

    std::shared_ptr<ASTNode> parsePostfix() {
        auto expr = parsePrimary();
        while (true) {
            if (check(TokenType::OP_INC) || check(TokenType::OP_DEC)) {
                Token op = consume();
                auto node = makeNode("PostfixOp", op.value, op.line, op.column);
                node->children.push_back(expr);
                expr = node;
            } else if (check(TokenType::LPAREN)) {
                consume();
                auto call = makeNode("FuncCall", expr->value, expr->line, expr->column);
                call->children.push_back(expr);
                if (!check(TokenType::RPAREN)) {
                    call->children.push_back(parseExpression());
                    while (match(TokenType::COMMA)) call->children.push_back(parseExpression());
                }
                expect(TokenType::RPAREN, "Expected ')'");
                expr = call;
            } else if (check(TokenType::LBRACKET)) {
                consume();
                auto idx = makeNode("ArrayAccess", "", expr->line, expr->column);
                idx->children.push_back(expr);
                idx->children.push_back(parseExpression());
                expect(TokenType::RBRACKET, "Expected ']'");
                expr = idx;
            } else if (check(TokenType::OP_DOT) || check(TokenType::OP_ARROW)) {
                Token op = consume();
                auto acc = makeNode("MemberAccess", op.value, op.line, op.column);
                acc->children.push_back(expr);
                Token member = expect(TokenType::IDENTIFIER, "Expected member name");
                acc->children.push_back(makeNode("Identifier", member.value, member.line, member.column));
                expr = acc;
            } else break;
        }
        return expr;
    }

    std::shared_ptr<ASTNode> parsePrimary() {
        if (check(TokenType::IDENTIFIER)) {
            Token t = consume();
            return makeNode("Identifier", t.value, t.line, t.column);
        }
        if (check(TokenType::INT_LITERAL)) {
            Token t = consume();
            return makeNode("IntLiteral", t.value, t.line, t.column);
        }
        if (check(TokenType::FLOAT_LITERAL)) {
            Token t = consume();
            return makeNode("FloatLiteral", t.value, t.line, t.column);
        }
        if (check(TokenType::CHAR_LITERAL)) {
            Token t = consume();
            return makeNode("CharLiteral", t.value, t.line, t.column);
        }
        if (check(TokenType::STRING_LITERAL)) {
            Token t = consume();
            return makeNode("StringLiteral", t.value, t.line, t.column);
        }
        if (match(TokenType::LPAREN)) {
            auto expr = parseExpression();
            expect(TokenType::RPAREN, "Expected ')'");
            auto paren = makeNode("Paren", "", expr->line, expr->column);
            paren->children.push_back(expr);
            return paren;
        }
        errors.push_back({"Expected expression, got '" + current().value + "'", current().line, current().column});
        Token t = consume();
        return makeNode("Error", t.value, t.line, t.column);
    }
};

static std::string escapeJson(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

std::string astToJson(const std::shared_ptr<ASTNode>& node, int indent) {
    if (!node) return "null";
    std::string pad(indent * 2, ' ');
    std::string childPad((indent + 1) * 2, ' ');
    std::ostringstream ss;
    ss << "{\n";
    ss << childPad << "\"type\": \"" << escapeJson(node->type) << "\",\n";
    ss << childPad << "\"value\": \"" << escapeJson(node->value) << "\",\n";
    ss << childPad << "\"line\": " << node->line << ",\n";
    ss << childPad << "\"column\": " << node->column << ",\n";
    ss << childPad << "\"children\": [";
    if (!node->children.empty()) {
        ss << "\n";
        for (size_t i = 0; i < node->children.size(); i++) {
            ss << childPad << "  " << astToJson(node->children[i], indent + 2);
            if (i + 1 < node->children.size()) ss << ",";
            ss << "\n";
        }
        ss << childPad;
    }
    ss << "]\n";
    ss << pad << "}";
    return ss.str();
}

ParseResult parse(const std::vector<Token>& tokens) {
    Parser parser(tokens);
    auto root = parser.parseProgram();
    return {root, parser.errors};
}
