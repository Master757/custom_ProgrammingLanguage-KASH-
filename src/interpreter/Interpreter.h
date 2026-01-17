#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <variant>

#include "../parser/AST.h"

class Interpreter {
public:
    void interpret(const std::vector<std::unique_ptr<Stmt>>& program);

private:
    // env now stores Value[which is dynamic] instead of int or double
    std::unordered_map<std::string, Value> env;

    void execute(const Stmt* stmt);

    // evaluate now returns Value
    Value evaluate(const Expr* expr);
};
