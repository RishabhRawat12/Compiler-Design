#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <iomanip>
#include "src/lexical/token.h"

using namespace std;

struct Symbol
{
    string type; // int char string
    int line;
    string scope; // global
};

class SymbolTable
{
public:
    map<string, Symbol> table;

    void insert(string name, string type, int line)
    {
        cout << "DEBUG: Inserting " << name << " into table!" << endl;
        table[name] = {type, line, "GLOBAL"};
    }

    bool lookup(string  name){
        return table.find(name) != table.end();
    }
    // Isse hum file ya console dono pe print kar sakte hain
    void display(ostream &out)
    {
        out << "\n"
            << string(50, '=') << endl;
        out << left << setw(15) << "NAME"
            << setw(15) << "TYPE"
            << setw(10) << "LINE"
            << setw(10) << "SCOPE" << endl;
        out << string(50, '-') << endl;

        for (auto const &pair : table)
        {
            string name =pair.first;
            Symbol sym=pair.second;
            out << left << setw(15) << name
                << setw(15) << sym.type
                << setw(10) << sym.line
                << setw(10) << sym.scope << endl;
        }
        out << string(50, '=') << endl;
    }
};

#endif
