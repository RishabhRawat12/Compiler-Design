#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "src/Lexical/lexer.h"
#include "src/Lexical/token.h"

using namespace std;

// Helper function to convert enum to string for the output file
// If the external asks, tell him: "Sir, C++ doesn't support enum reflection, so I mapped them manually."
string getTokenTypeName(TokenType type) {
    switch(type) {
        case KEYWORD_INT: return "KEYWORD_INT";
        case KEYWORD_CHAR: return "KEYWORD_CHAR";
        case KEYWORD_STRING: return "KEYWORD_STRING";
        case IDENTIFIER: return "IDENTIFIER";
        case STRING_LITERAL: return "STRING_LITERAL";
        case NUMBER: return "NUMBER";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case MULTIPLY: return "MULTIPLY";
        case DIVIDE: return "DIVIDE";
        case ASSIGN: return "ASSIGN";
        case SEMICOLON: return "SEMICOLON";
        case END_OF_FILE: return "END_OF_FILE";
        case UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

int main() {
    string inputPath = "input/program.txt";
    string outputPath = "output/tokens.txt";

    ifstream inputFile(inputPath);
    
    stringstream buffer;
    buffer << inputFile.rdbuf();
    string source = buffer.str();
    
    inputFile.close();

    //Initializing  the Lexer
    Lexer lexer(source);

    // Performing Lexical Analysis
    vector<Token> tokenList = lexer.tokenize();

    ofstream outputFile(outputPath);

    //Writeing Tokens to File

    for (int i = 0; i < tokenList.size(); i++) {
        Token t = tokenList[i];
        outputFile << getTokenTypeName(t.type) << "\t\t" << t.lexeme << "\t\t" << "line " << t.line << "\n";
                   
        // cout << getTokenTypeName(t.type) << " | " << t.lexeme << endl; // left this for debugging
    }

    outputFile.close();

    cout << "Lexical analysis done" << endl;
    cout << "Tokens generated" << endl;

    return 0;
}