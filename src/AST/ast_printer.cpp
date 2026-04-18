#include "ast_printer.h"
#include "stmt.h"

using json = nlohmann::json;

json ASTPrinter::buildAST(const std::vector<std::unique_ptr<Stmt>>& program) {
    json result = json::array();

    for (const auto& stmt : program) {
        if (stmt) {
            result.push_back(stmt->toJson());
        }
    }

    return result;
}