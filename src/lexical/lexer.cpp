#include "lexer.h"
#include<cctype> // needed for isAlpha, is Digit and isalnum
#include<vector>

//Constructor implemention

Lexer::Lexer(const string& src){
    source=src;
    start=0;
    current=0;
    line=1;
    col=1;
}

bool Lexer::isAtend(){
    return current >= source.length();
}

//advance
char Lexer:: advance(){
    char ch=source[current++];
    col++;
    return ch; //increment after feteching
}

//peek
char Lexer::peek(){
    if(isAtend()) 
        return '\0'; //null char if we hit the end
    
    return source[current];
}

char Lexer::peekNext(){
    if(current+1 >=source.length()) return '\0';

    return source[current+1];
}

void Lexer::addToken(TokenType type){

    //we extract exact lexeme using two pointers
    string text =source.substr(start,current - start);
    tokens.push_back(Token(type,text,line,col-(int)text.length()));
}

//identifier
void Lexer::identifier(){
    // point till the last char is alphanumeric
    while(isalnum(peek()) ||peek()== '_'){
        advance();
    }

    string text= source.substr(start,current-start);

    TokenType type;
    if(text=="int")
        type= KEYWORD_INT;
    else if(text=="char")
        type=KEYWORD_CHAR;
    else if(text=="string")
        type=KEYWORD_STRING;
    else
        type=IDENTIFIER;

    addToken(type);
}

//number
void Lexer:: number(){
    while(isdigit(peek())){
        advance();
    }

    addToken(NUMBER);
}

//string literal identifiaction
void Lexer::stringLiteral(){
    while(peek()!='"' && !isAtend()){
        if(peek()=='\n'){
            line++;
            col=1;
        }
        advance();
    }
    if(isAtend()){
        cerr<<"LEXER ERROR line "<<line<<" unterminatedd strong"<<endl;
        return;
    }
    advance();
    addToken(STRING_LITERAL);
}
//scann token
void Lexer::scanToken(){
    start=current;
    char c=advance();

    switch ((c))
    {
        //single char tokens
        case '+': addToken(PLUS); break;
        case '-': addToken(MINUS); break;
        case '*': addToken(MULTIPLY); break;

        case '=': addToken(ASSIGN); break;
        case ';': addToken(SEMICOLON); break;
        
        case '/':
            if(peek()=='/'){//if its a comment
                while(peek()!='\n'&& !isAtend())
                    advance();
            }
            else if(peek()=='*'){
                advance();

                while(!isAtend() && !(peek()=='*' && peekNext()=='/')){
                    if(peek()=='\n'){
                        line++;
                        col=1;
                    }
                    advance();
                }
                if(!isAtend()){
                    advance();// *
                    advance();// /
                }
            }
            else//it was a divide
                addToken(DIVIDE); break;

        case '"'://string literal
            stringLiteral();
            break;

        case '\'' : //char literals
            if(!isAtend())advance();
            if(peek()=='\''){
                advance();
                addToken(STRING_LITERAL);
            }
            else
                cerr<<"Invalid char liteal"<<endl;
            break;


        //ignore whitepsce
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n': line++;
            col=1;
            break;
    default:
        //identifiers or keywords
        if(isalpha(c)||c=='_')
            identifier();
        
        //numbers
        else if(isdigit(c)){
            number();
        }
        //invalid characters        
        else{
            cerr<<"Lexer error line "<<line<<" col "<<col<<" undefiend character"<<endl;
            addToken(UNKNOWN);
        }
        break;
    }
}

vector<Token> Lexer::tokenize(){
    while(!isAtend()){
        start=current; //to sync pointers for the new token
        scanToken();
    }

    //after everything appending end of file
    tokens.push_back(Token(END_OF_FILE,"EOF",line,col));

    return tokens;
}