#include "parser.h"

// Constructor Pointer aur tokens initialize karne ke liye
Parser::Parser(vector<Token> tokens,SymbolTable& st) :symTable(st)
{
    this->tokens = tokens;
    this->current = 0;

}

// Main function Pura program yahan se parse hota hai
vector<Stmt *> Parser::parse()
{
    vector<Stmt *> statements;
    while (!isAtEnd())
    {
        // Faltu semicolons ya empty lines skip karne ke liye
        if (match(SEMICOLON))
            continue;

        statements.push_back(declaration());
    }
    return statements;
}

//Declaration Logic: int x = 10; | string s = "something";
Stmt *Parser::declaration()
{
    if (match(KEYWORD_INT) || match(KEYWORD_CHAR) || match(KEYWORD_STRING))
    {
        Token typeToken=previous();
        Token name = consume(IDENTIFIER, "Give name to the variable");

        if(symTable.lookup(name.lexeme)){
            error(name,"Variable "+name.lexeme+" is already declared\n");
        }

        
        //inserting in the symbol table
        symTable.insert(name.lexeme,typeToken.getTypeName(), name.line);
    
        consume(ASSIGN, "Assignment operator = is missing");

        Expr *val = expression();
        consume(SEMICOLON, "add semicolon at the end of line");

        return new DeclarationStmt(name.lexeme, val);
    }

    return statement();
}

//Statement Routing
Stmt *Parser::statement()
{
    return assignment();
}

// Assignment Logic, x = 20;
Stmt *Parser::assignment()
{
    Token name = consume(IDENTIFIER, "Expect identifier for assignment.");

    if(!symTable.lookup(name.lexeme)){
            error(name,"Variable "+name.lexeme+" not declared\n");
        }

    consume(ASSIGN, "Expect = after variable name.");

    Expr *val = expression();
    consume(SEMICOLON, "Expect ; after assignment value.");

    return new AssignmentStmt(name.lexeme, val);
}


//Sabse low priority (+, -)
Expr *Parser::expression()
{
    return term();
}

//Term handles the addition and subtraction
Expr *Parser::term()
{
    Expr *expr = factor();

    while (match(PLUS) || match(MINUS))
    {
        string op = previous().lexeme;
        Expr *right = factor();
        expr = new BinaryExpr(expr, op, right);
    }
    return expr;
}

//Factor handling  multiplication and division
Expr *Parser::factor()
{
    Expr *expr = primary();

    while (match(MULTIPLY) || match(DIVIDE))
    {
        string op = previous().lexeme;
        Expr *right = primary();
        expr = new BinaryExpr(expr, op, right);
    }
    return expr;
}

//Primary (Numbers, Strings, or Variables)

Expr* Parser::primary() {
    if (match(NUMBER)) {
        return new NumberExpr(stoi(previous().lexeme));
    }
    
    if (match(STRING_LITERAL)) {
        return new StringExpr(previous().lexeme);
    }
    
    if (match(CHAR_LITERAL)) {
        return new CharExpr(previous().lexeme);
    }
    
    if (match(IDENTIFIER)) {
        Token var=previous();

        //if variable is not already decalred
        if(!symTable.lookup(var.lexeme)){
            error(var, "Variable '" + var.lexeme + "' used without declaration!");
        }
        return new VariableExpr(var.lexeme);
    }
    
    error(peek(), "Invalid expression syntax");
    return nullptr;
}

// Helpers for Pointer Navigation

Token Parser::advance()
{
    if (!isAtEnd())
        current++;
    return previous();
}

bool Parser::isAtEnd()
{
    return peek().type == END_OF_FILE;
}

Token Parser::peek()
{
    return tokens[current];
}

Token Parser::previous()
{
    return tokens[current - 1];
}

bool Parser::check(TokenType type)
{
    if (isAtEnd())
        return false;
    return peek().type == type;
}

bool Parser::match(TokenType type)
{
    if (check(type))
    {
        advance();
        return true;
    }
    return false;
}

Token Parser::consume(TokenType type, string msg)
{
    if (check(type))
        return advance();

    error(peek(), msg);
    return Token();
}

// Error Handling
void Parser::error(Token token, string msg)
{
    cerr << "[Parser Error] Line " << token.line<< " at '" << token.lexeme << "': " << msg << endl;
    exit(1); // Seedha terminate, no time for recovery
}