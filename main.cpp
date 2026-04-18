#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>
#include "include/json.hpp"

#include "src/lexical/lexer.h"
#include "src/lexical/token.h"
#include "src/syntax/parser.h"
#include "src/AST/ast_printer.h"
#include "src/semantics/semantic_analyzer.h"
#include "src/icg/icg_generator.h"
#include "src/optimizer/optimizer.h"
#include "symbol_table.h"

using namespace std;
using json = nlohmann::json;

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        cout << "Missing arguments!" << endl;
        cout << "Usage: " << argv[0] << " <input_file> <token_out> <ast_out> <semantic_out> [tac_out]" << endl;
        return 1;
    }

    string inputPath = argv[1];
    string tokenOut = argv[2];
    string astOut = argv[3];
    string semOut = argv[4];
    string tacOut = (argc > 5) ? argv[5] : "output/intermediate.txt";
    string tacRawOut = "output/intermediate_raw.txt";

    ifstream file(inputPath);
    if (!file.is_open())
        return 1;

    stringstream stream;
    stream << file.rdbuf();
    string code = stream.str();
    file.close();

    // PHASE 1: Lexing
    Lexer myLexer(code);
    vector<Token> tokens = myLexer.tokenize();
    vector<LexError> lexErrors = myLexer.getErrors();

    json tokenJson = json::array();
    for (const auto &t : tokens)
        tokenJson.push_back(t.toJson());

    ofstream tFile(tokenOut);
    if (tFile.is_open())
    {
        json out;
        out["tokens"] = tokenJson;
        json errs = json::array();
        for (const auto &e : lexErrors)
            errs.push_back(e.toJson());
        out["errors"] = errs;
        tFile << out.dump(4);
        tFile.close();
    }

    // PHASE 2: Parsing
    SymbolTable table;
    Parser myParser(tokens, table);
    vector<unique_ptr<Stmt>> ast = myParser.parse();
    vector<ParseError> parseErrors = myParser.getErrors();

    ofstream aFile(astOut);
    if (aFile.is_open())
    {
        json out;
        out["ast"] = ASTPrinter::buildAST(ast);
        json errs = json::array();
        for (const auto &e : parseErrors)
        {
            errs.push_back({{"line", e.line}, {"col", e.col}, {"message", e.message}});
        }
        out["errors"] = errs;
        aFile << out.dump(4);
        aFile.close();
    }

    // PHASE 3: Semantics
    SemanticAnalyzer checker(table);
    if (parseErrors.empty()) {
        checker.analyze(ast);
    }

    bool hasErrors = false;
    for (const auto& e : checker.getErrors()) {
        if (e.message.find("WARNING:") == string::npos) {
            hasErrors = true;
            break;
        }
    }

    // Save Semantic results and the final symbol table
    ofstream sFile(semOut);
    if (sFile.is_open()) {
        json out;
        out["semanticErrors"] = checker.getErrorsAsJson();
        
        json symTable = json::object();
        for (auto const& [name, info] : table.table) {
            symTable[name] = {
                {"type", info.type},
                {"line", info.line},
                {"scope", info.scope},
                {"isFunction", info.isFunction},
                {"paramTypes", info.paramTypes} // Added new fields here
            };
        }
        out["symbolTable"] = symTable;
        sFile << out.dump(4);
        sFile.close();
    }

    // PHASE 4: Intermediate Code Generation
    if (parseErrors.empty() && !hasErrors) {
        ICGGenerator icg;
        icg.generate(ast);
        vector<TACInstruction> rawInstructions = icg.getInstructions();

        // Output Raw TAC first
        ofstream rFile(tacRawOut);
        if (rFile.is_open()) {
            for (const auto& inst : rawInstructions) {
                rFile << inst.toString() << "\n";
            }
            rFile.close();
            cout << "Raw TAC generated in " << tacRawOut << endl;
        }

        // PHASE 5: Optimization
        Optimizer opt;
        vector<TACInstruction> optimizedInstructions = opt.optimize(rawInstructions);
        
        ofstream iFile(tacOut);
        if (iFile.is_open()) {
            for (const auto& inst : optimizedInstructions) {
                iFile << inst.toString() << "\n";
            }
            iFile.close();
            cout << "Optimized TAC generated in " << tacOut << endl;
            if (optimizedInstructions.size() < rawInstructions.size()) {
                cout << "Optimization reduced instructions from " << rawInstructions.size() 
                     << " to " << optimizedInstructions.size() << endl;
            }
        }
    }

    cout << "Done! Everything processed." << endl;
    return 0;
}