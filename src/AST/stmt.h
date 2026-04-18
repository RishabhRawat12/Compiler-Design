#ifndef STMT_H
#define STMT_H

#include "ast.h"
#include "expr.h"
#include <vector>
#include <memory>

class DeclarationStmt : public Stmt {
public:
    string varType;
    string name;
    std::unique_ptr<Expr> initializer;
    DeclarationStmt(string vType, string n, std::unique_ptr<Expr> init, int l)
        : varType(vType), name(n), initializer(move(init)) { line = l; }
    json toJson() const override {
        return { {"type", "DeclarationStmt"}, {"varType", varType}, {"name", name}, {"initializer", initializer ? initializer->toJson() : json()}, {"line", line} };
    }
};

class AssignmentStmt : public Stmt {
public:
    std::unique_ptr<Expr> target;
    std::unique_ptr<Expr> value;
    AssignmentStmt(std::unique_ptr<Expr> t, std::unique_ptr<Expr> v, int l) : target(move(t)), value(move(v)) { line = l; }
    json toJson() const override {
        return { {"type", "AssignmentStmt"}, {"target", target ? target->toJson() : json()}, {"value", value ? value->toJson() : json()}, {"line", line} };
    }
};

class IfStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Stmt>> thenBranch;
    std::vector<std::unique_ptr<Stmt>> elseBranch;
    IfStmt(std::unique_ptr<Expr> c, std::vector<std::unique_ptr<Stmt>> t, std::vector<std::unique_ptr<Stmt>> e, int l)
        : condition(move(c)), thenBranch(move(t)), elseBranch(move(e)) { line = l; }
    json toJson() const override {
        json thenJson = json::array(), elseJson = json::array();
        for (const auto& stmt : thenBranch) thenJson.push_back(stmt->toJson());
        for (const auto& stmt : elseBranch) elseJson.push_back(stmt->toJson());
        return { {"type", "IfStmt"}, {"condition", condition ? condition->toJson() : json()}, {"thenBranch", thenJson}, {"elseBranch", elseJson}, {"line", line} };
    }
};

class ForStmt : public Stmt {
public:
    std::unique_ptr<Stmt> init;
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> increment;
    std::vector<std::unique_ptr<Stmt>> body;
    ForStmt(std::unique_ptr<Stmt> i, std::unique_ptr<Expr> c, std::unique_ptr<Stmt> inc, std::vector<std::unique_ptr<Stmt>> b, int l)
        : init(move(i)), condition(move(c)), increment(move(inc)), body(move(b)) { line = l; }
    json toJson() const override {
        json bodyJson = json::array();
        for (const auto& stmt : body) bodyJson.push_back(stmt->toJson());
        return { {"type", "ForStmt"}, {"init", init ? init->toJson() : json()}, {"condition", condition ? condition->toJson() : json()}, {"increment", increment ? increment->toJson() : json()}, {"body", bodyJson}, {"line", line} };
    }
};

class FunctionStmt : public Stmt {
public:
    string returnType;
    string name;
    std::vector<std::pair<string, string>> params;
    std::vector<std::unique_ptr<Stmt>> body;
    FunctionStmt(string rType, string n, std::vector<std::pair<string, string>> p, std::vector<std::unique_ptr<Stmt>> b, int l)
        : returnType(rType), name(n), params(move(p)), body(move(b)) { line = l; }
    json toJson() const override {
        json pJson = json::array(), bJson = json::array();
        for (const auto& p : params) pJson.push_back({ {"type", p.first}, {"name", p.second} });
        for (const auto& s : body) bJson.push_back(s->toJson());
        return { {"type", "FunctionStmt"}, {"returnType", returnType}, {"name", name}, {"params", pJson}, {"body", bJson}, {"line", line} };
    }
};

class ReturnStmt : public Stmt {
public:
    std::unique_ptr<Expr> value;
    ReturnStmt(std::unique_ptr<Expr> v, int l) : value(move(v)) { line = l; }
    json toJson() const override { return { {"type", "ReturnStmt"}, {"value", value ? value->toJson() : json()}, {"line", line} }; }
};

class BreakStmt : public Stmt {
public:
    BreakStmt(int l) { line = l; }
    json toJson() const override { return { {"type", "BreakStmt"}, {"line", line} }; }
};

class ContinueStmt : public Stmt {
public:
    ContinueStmt(int l) { line = l; }
    json toJson() const override { return { {"type", "ContinueStmt"}, {"line", line} }; }
};

#endif