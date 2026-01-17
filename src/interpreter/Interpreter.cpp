#include "Interpreter.h"
#include <iostream>
#include <type_traits>
#include <stdexcept>
#include <sstream>    
#include <cmath>  

struct BreakSignal {};

void Interpreter::interpret(const std::vector<std::unique_ptr<Stmt>>& program) {
    try {
        for (const auto& stmt : program) {
            execute(stmt.get());
        }
    } catch (BreakSignal&) {
        throw std::runtime_error("break used outside of a loop");
    }
}

static bool isIntValue(const Value& v) {
    return std::holds_alternative<int>(v);
}
static bool isDoubleValue(const Value& v) {
    return std::holds_alternative<double>(v);
}
static bool isStringValue(const Value& v) {
    return std::holds_alternative<std::string>(v);
}

static double toDouble(const Value& v) {
    if (std::holds_alternative<double>(v)) return std::get<double>(v);
    if (std::holds_alternative<int>(v)) return static_cast<double>(std::get<int>(v));
    throw std::runtime_error("Value is not numeric");
}

static int toIntChecked(const Value& v) {
    if (std::holds_alternative<int>(v)) return std::get<int>(v);
    if (std::holds_alternative<double>(v)) return static_cast<int>(std::get<double>(v));
    throw std::runtime_error("Value is not numeric");
}

void Interpreter::execute(const Stmt* stmt) {


    // block systems
    if (auto block = dynamic_cast<const BlockStmt*>(stmt)) {
        for (const auto& s : block->statements) {
            execute(s.get());
        }
        return;
    }
   //break statement
    if (dynamic_cast<const BreakStmt*>(stmt)) {
    throw BreakSignal{};
    return;
}


    // if (condition)
    if (auto ifStmt = dynamic_cast<const IfStmt*>(stmt)) {
        Value condVal = evaluate(ifStmt->condition.get());

        bool condTrue = false;
        if (isIntValue(condVal)) {
            condTrue = (std::get<int>(condVal) != 0);
        } else if (isDoubleValue(condVal)) {
            condTrue = (std::get<double>(condVal) != 0.0);
        } else {
            throw std::runtime_error("If condition must be a number (int or float)");
        }

        if (condTrue) {
            for (const auto& s : ifStmt->thenBody) {
                execute(s.get());
            }
        } else {
            for (const auto& s : ifStmt->elseBody) {
                execute(s.get());
            }
        }
        return;
    }

    // while (condition) 
    if (auto whileStmt = dynamic_cast<const WhileStmt*>(stmt)) {
        while (true) {
            Value condVal = evaluate(whileStmt->condition.get());

            if (!std::holds_alternative<int>(condVal)) {
                throw std::runtime_error("While condition must be an integer");
            }

            if (std::get<int>(condVal) == 0) break;

            try {
                for (const auto& s : whileStmt->body) {
                    execute(s.get());
                }
            } catch (BreakSignal&) {
                break;
            }
    }
    return;
}


    // out(expression) to print things to the terminl;
    if (auto printStmt = dynamic_cast<const PrintStmt*>(stmt)) {
        Value val = evaluate(printStmt->expression.get());

        std::visit([](auto&& arg) {
    using T = std::decay_t<decltype(arg)>;

    if constexpr (std::is_same_v<T, double>) {
        if (arg == static_cast<long long>(arg)) {
            std::cout << arg << ".0";
        } else {
            std::cout << arg;
        }
    } else {
        std::cout << arg;
    }}, val);


        std::cout << std::endl;
        return;
    }

    // in(identifier) this is for inputting
    if (auto inputStmt = dynamic_cast<const InputStmt*>(stmt)) {
        std::string input;
        std::getline(std::cin, input);

        // handle leftover newline
        if (input.empty() && std::cin.good()) {
            std::getline(std::cin, input);
        }
        env[inputStmt->name] = input;
        return;
    }

    // assignment
    if (auto assignStmt = dynamic_cast<const AssignStmt*>(stmt)) {
        Value val = evaluate(assignStmt->expression.get());
        env[assignStmt->name] = val;
        return;
    }

    throw std::runtime_error("Unknown statement type");
}

Value Interpreter::evaluate(const Expr* expr) {
     if (auto lit = dynamic_cast<const literalExpressions*>(expr)) {
        return lit->val;
    }

    if (auto s = dynamic_cast<const StringExpr*>(expr)) {
        return s->val;
    }


    // Variable managements
    if (auto var = dynamic_cast<const VariableExpr*>(expr)) {
        if (env.count(var->n) == 0) {
            throw std::runtime_error("Undefined variable: " + var->n);
        }
        return env[var->n];
    }

    // Binary expression [handls all binary operations]
    if (auto bin = dynamic_cast<const BinaryExpr*>(expr)) {
        Value left = evaluate(bin->left.get());
        Value right = evaluate(bin->right.get());

        // PLUS: int+int OR string+string OR numeric promotion to double
        if (bin->op == TokenTypes::PLUS) {
            // both ints
            if (isIntValue(left) && isIntValue(right)) {
                return std::get<int>(left) + std::get<int>(right);
            }

            // both strings
            if (isStringValue(left) && isStringValue(right)) {
                return std::get<std::string>(left) + std::get<std::string>(right);
            }

            // numeric promotion: if both numeric but one is double -> double result
            if ((isIntValue(left) || isDoubleValue(left)) &&
                (isIntValue(right) || isDoubleValue(right))) {

                if (isDoubleValue(left) || isDoubleValue(right)) {
                    double l = toDouble(left);
                    double r = toDouble(right);
                    return l + r;
                } else { // both int handled earlier, but keep safe
                    return std::get<int>(left) + std::get<int>(right);
                }
            }

            throw std::runtime_error("Type error: '+' requires operands of same type or both numeric");
        }

        // Arithmetic operations (-, *, /, %)
        if (bin->op == TokenTypes::MINUS ||
            bin->op == TokenTypes::ASTERISK ||
            bin->op == TokenTypes::SLASH ||
            bin->op == TokenTypes::MODULUS) {

            // checking for vlidity
            if (!((isIntValue(left) || isDoubleValue(left)) &&
                  (isIntValue(right) || isDoubleValue(right)))) {
                throw std::runtime_error("Arithmetic operators require numbers");
            }

            // if either is double --> do double math
            if (isDoubleValue(left) || isDoubleValue(right)) {
                double l = toDouble(left);
                double r = toDouble(right);

                switch (bin->op) {
                    case TokenTypes::MINUS:    return l - r;
                    case TokenTypes::ASTERISK: return l * r;
                    case TokenTypes::SLASH:
                        if (r == 0.0) throw std::runtime_error("Division by zero");
                        return l / r;
                    case TokenTypes::MODULUS:
                        throw std::runtime_error("Modulo not supported for floats");
                    default: break;
                }
            } else {
                // both ints -> integer arithmetic
                int l = std::get<int>(left);
                int r = std::get<int>(right);

                switch (bin->op) {
                    case TokenTypes::MINUS:    return l - r;
                    case TokenTypes::ASTERISK: return l * r;
                    case TokenTypes::SLASH:
                        if (r == 0) throw std::runtime_error("Division by zero");
                        return l / r; // integer division
                    case TokenTypes::MODULUS:
                        if (r == 0) throw std::runtime_error("Modulo by zero");
                        return l % r;
                    default: break;
                }
            }
        }

        // Comparisons (return 1 or 0) --> boolean is still not integrated
    
        if (bin->op == TokenTypes::EQUAL_EQUAL ||
            bin->op == TokenTypes::NOT_EQUAL ||
            bin->op == TokenTypes::GREATER ||
            bin->op == TokenTypes::LESSER ||
            bin->op == TokenTypes::GREATER_EQUAL ||
            bin->op == TokenTypes::LESSER_EQUAL) {

            // Numeric comparisons (ints or doubles)
            if ((isIntValue(left) || isDoubleValue(left)) &&
                (isIntValue(right) || isDoubleValue(right))) {

                // force double to be higherarche
                if (isDoubleValue(left) || isDoubleValue(right)) {
                    double l = toDouble(left);
                    double r = toDouble(right);

                    switch (bin->op) {
                        case TokenTypes::EQUAL_EQUAL:   return (l == r) ? 1 : 0;
                        case TokenTypes::NOT_EQUAL:     return (l != r) ? 1 : 0;
                        case TokenTypes::GREATER:       return (l >  r) ? 1 : 0;
                        case TokenTypes::LESSER:        return (l <  r) ? 1 : 0;
                        case TokenTypes::GREATER_EQUAL: return (l >= r) ? 1 : 0;
                        case TokenTypes::LESSER_EQUAL:  return (l <= r) ? 1 : 0;
                        default: break;
                    }
                } else {
                    int l = std::get<int>(left);
                    int r = std::get<int>(right);

                    switch (bin->op) {
                        case TokenTypes::EQUAL_EQUAL:   return (l == r) ? 1 : 0;
                        case TokenTypes::NOT_EQUAL:     return (l != r) ? 1 : 0;
                        case TokenTypes::GREATER:       return (l >  r) ? 1 : 0;
                        case TokenTypes::LESSER:        return (l <  r) ? 1 : 0;
                        case TokenTypes::GREATER_EQUAL: return (l >= r) ? 1 : 0;
                        case TokenTypes::LESSER_EQUAL:  return (l <= r) ? 1 : 0;
                        default: break;
                    }
                }
            }

            // string equality checking things
            if (isStringValue(left) && isStringValue(right)) {
                const std::string& l = std::get<std::string>(left);
                const std::string& r = std::get<std::string>(right);

                if (bin->op == TokenTypes::EQUAL_EQUAL) {
                    return (l == r) ? 1 : 0;
                }
                if (bin->op == TokenTypes::NOT_EQUAL) {
                    return (l != r) ? 1 : 0;
                }

                throw std::runtime_error("Only == and != allowed for strings");
            }

            throw std::runtime_error("Type mismatch in comparison");
        }

        throw std::runtime_error("Unknown binary operator");
    }

    if (auto call = dynamic_cast<const CallExpr*>(expr)) {
        Value arg = evaluate(call->argument.get());

        // toString(expr)
        if (call->callee == "toString") {
            if (isIntValue(arg)) {
                return std::to_string(std::get<int>(arg));
            }
            if (isDoubleValue(arg)) {
                return std::to_string(std::get<double>(arg));
            }
            if (isStringValue(arg)) {
                return arg;
            }
            throw std::runtime_error("toString: unsupported type");
        }

        // toNum(expr) -> try to parse as double, return int if whole number
        if (call->callee == "toNum") {
            if (isIntValue(arg) || isDoubleValue(arg)) {
                return arg;
            }
            if (isStringValue(arg)) {
                const std::string& s = std::get<std::string>(arg);
                try {
                    double dv = std::stod(s);
                    double iv = std::floor(dv);
                    if (dv == iv) {
                        return static_cast<int>(iv);
                    } else {
                        return dv;
                    }
                } catch (...) {
                    throw std::runtime_error("toNum: cannot convert \"" + s + "\" to number");
                }
            }
            throw std::runtime_error("toNum: unsupported type");
        }

        // input() as expression
        if (call->callee == "input") {
            std::string s;
            std::getline(std::cin, s);
            if (s.empty() && std::cin.good()) std::getline(std::cin, s);
            return s;
        }

        throw std::runtime_error("Unknown function: " + call->callee);
    }

    throw std::runtime_error("Unknown expression type");
}
