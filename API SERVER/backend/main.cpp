#include <iostream>
#include <sstream>
#include <string>
#include "lexer/lexical_analyzer.h"
#include "parser/syntax_analyzer.h"
#include "semantic/semantic_analyzer.h"
#include "intermediate/intermediate_code.h"

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

static std::string tokensToJson(const std::vector<Token>& tokens) {
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == TokenType::END_OF_FILE) break;
        if (i > 0) ss << ",";
        ss << "{\"type\":\"" << escapeJson(tokens[i].typeName())
           << "\",\"value\":\"" << escapeJson(tokens[i].value)
           << "\",\"line\":" << tokens[i].line
           << ",\"column\":" << tokens[i].column << "}";
    }
    ss << "]";
    return ss.str();
}

static std::string lexErrorsToJson(const std::vector<LexicalError>& errs) {
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < errs.size(); i++) {
        if (i > 0) ss << ",";
        ss << "{\"message\":\"" << escapeJson(errs[i].message)
           << "\",\"line\":" << errs[i].line
           << ",\"column\":" << errs[i].column << "}";
    }
    ss << "]";
    return ss.str();
}

static std::string syntaxErrorsToJson(const std::vector<SyntaxError>& errs) {
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < errs.size(); i++) {
        if (i > 0) ss << ",";
        ss << "{\"message\":\"" << escapeJson(errs[i].message)
           << "\",\"line\":" << errs[i].line
           << ",\"column\":" << errs[i].column << "}";
    }
    ss << "]";
    return ss.str();
}

static std::string semanticErrorsToJson(const std::vector<SemanticError>& errs) {
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < errs.size(); i++) {
        if (i > 0) ss << ",";
        ss << "{\"message\":\"" << escapeJson(errs[i].message)
           << "\",\"line\":" << errs[i].line
           << ",\"column\":" << errs[i].column
           << ",\"severity\":\"" << escapeJson(errs[i].severity) << "\"}";
    }
    ss << "]";
    return ss.str();
}

static std::string warningsToJson(const std::vector<SemanticWarning>& warns) {
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < warns.size(); i++) {
        if (i > 0) ss << ",";
        ss << "{\"message\":\"" << escapeJson(warns[i].message)
           << "\",\"line\":" << warns[i].line
           << ",\"column\":" << warns[i].column
           << ",\"category\":\"" << escapeJson(warns[i].category) << "\"}";
    }
    ss << "]";
    return ss.str();
}

static std::string symbolTableToJson(const std::vector<SymbolEntry>& symbols) {
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < symbols.size(); i++) {
        if (i > 0) ss << ",";
        ss << "{\"name\":\"" << escapeJson(symbols[i].name)
           << "\",\"type\":\"" << escapeJson(symbols[i].type)
           << "\",\"scope\":\"" << escapeJson(symbols[i].scope)
           << "\",\"declarationLine\":" << symbols[i].declarationLine
           << ",\"isUsed\":" << (symbols[i].isUsed ? "true" : "false")
           << ",\"isFunction\":" << (symbols[i].isFunction ? "true" : "false") << "}";
    }
    ss << "]";
    return ss.str();
}

static std::string irToJson(const std::vector<TACInstruction>& instructions) {
    std::ostringstream ss;
    ss << "[";
    for (size_t i = 0; i < instructions.size(); i++) {
        if (i > 0) ss << ",";
        ss << "\"" << escapeJson(instructions[i].toString()) << "\"";
    }
    ss << "]";
    return ss.str();
}

static std::string suggestionsToJson(const SemanticResult& sem, const std::vector<Token>& tokens) {
    std::ostringstream ss;
    ss << "[";
    bool first = true;

    auto addSuggestion = [&](const std::string& msg) {
        if (!first) ss << ",";
        ss << "\"" << escapeJson(msg) << "\"";
        first = false;
    };

    if (sem.cyclomaticComplexity > 10) {
        addSuggestion("High cyclomatic complexity (" + std::to_string(sem.cyclomaticComplexity) +
                      "). Consider splitting complex functions into smaller ones.");
    }
    if (sem.maxNestingDepth > 4) {
        addSuggestion("Deep nesting detected (depth " + std::to_string(sem.maxNestingDepth) +
                      "). Consider using early returns or helper functions.");
    }
    for (auto& w : sem.warnings) {
        if (w.category == "unused-variable") {
            addSuggestion("Remove unused variable or use it: " + w.message);
        }
    }
    if (!sem.errors.empty()) {
        addSuggestion("Fix semantic errors before proceeding: undeclared variables can cause runtime crashes.");
    }

    ss << "]";
    return ss.str();
}

int main() {
    // Read all input from stdin
    std::string source((std::istreambuf_iterator<char>(std::cin)),
                        std::istreambuf_iterator<char>());

    // Phase 1: Lexical Analysis
    LexerResult lexResult = tokenize(source);

    // Phase 2: Syntax Analysis
    ParseResult parseResult = parse(lexResult.tokens);

    // Phase 3: Semantic Analysis
    SemanticResult semResult;
    if (parseResult.root) {
        semResult = analyzeSemantics(parseResult.root);
    }

    // Phase 4: Intermediate Code Generation
    IRResult irResult;
    if (parseResult.root && parseResult.errors.empty()) {
        irResult = generateIR(parseResult.root);
    }

    // Build JSON output
    std::ostringstream out;
    out << "{\n";
    out << "  \"tokens\": " << tokensToJson(lexResult.tokens) << ",\n";
    out << "  \"lexicalErrors\": " << lexErrorsToJson(lexResult.errors) << ",\n";
    out << "  \"syntaxErrors\": " << syntaxErrorsToJson(parseResult.errors) << ",\n";
    out << "  \"semanticErrors\": " << semanticErrorsToJson(semResult.errors) << ",\n";
    out << "  \"warnings\": " << warningsToJson(semResult.warnings) << ",\n";
    out << "  \"symbolTable\": " << symbolTableToJson(semResult.symbolTable) << ",\n";
    out << "  \"parseTree\": " << (parseResult.root ? astToJson(parseResult.root) : "null") << ",\n";
    out << "  \"intermediateCode\": " << irToJson(irResult.instructions) << ",\n";
    out << "  \"suggestions\": " << suggestionsToJson(semResult, lexResult.tokens) << ",\n";
    out << "  \"metrics\": {\"cyclomaticComplexity\":" << semResult.cyclomaticComplexity
        << ",\"maxNestingDepth\":" << semResult.maxNestingDepth
        << ",\"functionCount\":" << semResult.functionCount << "}\n";
    out << "}";

    std::cout << out.str() << std::endl;
    return 0;
}
