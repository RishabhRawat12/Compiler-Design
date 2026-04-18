#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <map>

using namespace std;

struct SymbolInfo {
    string type;
    int line;
    string scope;
    bool isFunction = false;
    vector<string> paramTypes;
};

class SymbolTable {
public:
    vector<map<string, SymbolInfo>> scopes;
    map<string, SymbolInfo> table; 

    SymbolTable() {
        scopes.push_back(map<string, SymbolInfo>());
    }

    void enterScope() {
        scopes.push_back(map<string, SymbolInfo>());
    }

    void exitScope() {
        if (scopes.size() > 1) scopes.pop_back();
    }

    bool lookupCurrentScope(string name) {
        return scopes.back().find(name) != scopes.back().end();
    }

    bool lookup(string name) {
        for (int i = scopes.size() - 1; i >= 0; i--) {
            if (scopes[i].find(name) != scopes[i].end()) return true;
        }
        return false;
    }

    SymbolInfo get(string name) {
        for (int i = scopes.size() - 1; i >= 0; i--) {
            if (scopes[i].find(name) != scopes[i].end()) return scopes[i][name];
        }
        return {"unknown", 0, "UNKNOWN", false, {}};
    }

    void insertWithHistory(string name, string type, int line) {
        string scopeName = (scopes.size() == 1) ? "GLOBAL" : "LOCAL";
        scopes.back()[name] = {type, line, scopeName, false, {}};
        table[name] = {type, line, scopeName, false, {}};
    }

    void insertFunctionWithHistory(string name, string type, int line, vector<string> pTypes) {
        string scopeName = (scopes.size() == 1) ? "GLOBAL" : "LOCAL";
        scopes.back()[name] = {type, line, scopeName, true, pTypes};
        table[name] = {type, line, scopeName, true, pTypes};
    }
};

#endif