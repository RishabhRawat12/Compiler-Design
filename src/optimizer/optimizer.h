#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include <string>
#include "../icg/tac.h"

using namespace std;

class Optimizer {
private:
    bool isNumber(const string& s);
    int toNumber(const string& s);

    // Specific optimization passes
    bool constantFolding(vector<TACInstruction>& code);
    bool algebraicSimplification(vector<TACInstruction>& code);
    bool copyPropagation(vector<TACInstruction>& code);
    bool deadTemporaryElimination(vector<TACInstruction>& code);

public:
    Optimizer() {}
    vector<TACInstruction> optimize(const vector<TACInstruction>& inputCode);
};

#endif
