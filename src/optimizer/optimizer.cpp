#include "optimizer.h"
#include <algorithm>
#include <map>

bool Optimizer::isNumber(const string& s) {
    if (s.empty()) return false;
    for (char const &c : s) {
        if (std::isdigit(c) == 0) return false;
    }
    return true;
}

int Optimizer::toNumber(const string& s) {
    return stoi(s);
}

vector<TACInstruction> Optimizer::optimize(const vector<TACInstruction>& inputCode) {
    vector<TACInstruction> code = inputCode;
    bool changed = true;
    
    // Perform multiple passes until no more optimizations can be made
    while (changed) {
        changed = false;
        changed |= constantFolding(code);
        changed |= algebraicSimplification(code);
        changed |= copyPropagation(code);
        changed |= deadTemporaryElimination(code);
    }
    
    return code;
}

bool Optimizer::deadTemporaryElimination(vector<TACInstruction>& code) {
    map<string, int> useCount;
    // Count how many times each variable is used as an operand
    for (const auto& inst : code) {
        if (!inst.arg1.empty()) useCount[inst.arg1]++;
        if (!inst.arg2.empty()) useCount[inst.arg2]++;
    }

    bool changed = false;
    vector<TACInstruction> newCode;
    for (const auto& inst : code) {
        // If the instruction assigns to a temporary variable (t0, t1...)
        // AND that temporary variable is never used, skip (delete) it.
        if (inst.result.length() > 0 && inst.result[0] == 't') {
            if (useCount[inst.result] == 0) {
                changed = true;
                continue; // Skip this instruction
            }
        }
        newCode.push_back(inst);
    }
    
    if (changed) code = newCode;
    return changed;
}

bool Optimizer::constantFolding(vector<TACInstruction>& code) {
    bool changed = false;
    for (auto& inst : code) {
        if (isNumber(inst.arg1) && isNumber(inst.arg2)) {
            int a = toNumber(inst.arg1);
            int b = toNumber(inst.arg2);
            int res = 0;
            bool folded = false;

            if (inst.op == "+") { res = a + b; folded = true; }
            else if (inst.op == "-") { res = a - b; folded = true; }
            else if (inst.op == "*") { res = a * b; folded = true; }
            else if (inst.op == "/" && b != 0) { res = a / b; folded = true; }

            if (folded) {
                inst.op = "=";
                inst.arg1 = to_string(res);
                inst.arg2 = "";
                changed = true;
            }
        }
    }
    return changed;
}

bool Optimizer::algebraicSimplification(vector<TACInstruction>& code) {
    bool changed = false;
    for (auto& inst : code) {
        // x + 0 = x
        if (inst.op == "+" && inst.arg2 == "0") {
            inst.op = "=";
            inst.arg2 = "";
            changed = true;
        }
        // 0 + x = x
        else if (inst.op == "+" && inst.arg1 == "0") {
            inst.op = "=";
            inst.arg1 = inst.arg2;
            inst.arg2 = "";
            changed = true;
        }
        // x - 0 = x
        else if (inst.op == "-" && inst.arg2 == "0") {
            inst.op = "=";
            inst.arg2 = "";
            changed = true;
        }
        // x * 1 = x
        else if (inst.op == "*" && inst.arg2 == "1") {
            inst.op = "=";
            inst.arg2 = "";
            changed = true;
        }
        // 1 * x = x
        else if (inst.op == "*" && inst.arg1 == "1") {
            inst.op = "=";
            inst.arg1 = inst.arg2;
            inst.arg2 = "";
            changed = true;
        }
        // x * 0 = 0
        else if (inst.op == "*" && (inst.arg1 == "0" || inst.arg2 == "0")) {
            inst.op = "=";
            inst.arg1 = "0";
            inst.arg2 = "";
            changed = true;
        }
    }
    return changed;
}

bool Optimizer::copyPropagation(vector<TACInstruction>& code) {
    bool changed = false;
    // A simple version: if we have t1 = a followed by b = t1, and t1 is a temp variable
    // We can change b = t1 to b = a
    for (size_t i = 0; i < code.size(); ++i) {
        if (code[i].op == "=" && code[i].result.length() > 0 && code[i].result[0] == 't') {
            string tempVar = code[i].result;
            string originalVar = code[i].arg1;

            // Look ahead for uses of tempVar
            for (size_t j = i + 1; j < code.size(); ++j) {
                // If tempVar is redefined, stop
                if (code[j].result == tempVar) break;

                // Propagate
                if (code[j].arg1 == tempVar) { code[j].arg1 = originalVar; changed = true; }
                if (code[j].arg2 == tempVar) { code[j].arg2 = originalVar; changed = true; }
                if (code[j].op == "=" && code[j].arg1 == tempVar) { code[j].arg1 = originalVar; changed = true; }
            }
        }
    }
    return changed;
}
