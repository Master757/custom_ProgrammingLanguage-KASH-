#pragma once

#include <memory>
#include <string>
#include <vector>
#include <variant>
#include "../lexer/Token.h"  


using Value = std::variant<int, double, std::string>;
struct Expr {
    virtual ~Expr() = default;
};

struct literalExpressions : Expr {
    Value val;
    literalExpressions(Value v) : val(v) {}
};

struct StringExpr : Expr {
    std::string val;
    StringExpr(const std::string &v) : val(v) {}
};

struct VariableExpr : Expr {
    std::string n;
    VariableExpr(const std::string  &n) : n(n) {}
};

struct BinaryExpr : Expr {
    TokenTypes op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    BinaryExpr(
        TokenTypes op,std::unique_ptr<Expr> left,std::unique_ptr<Expr> right): op(op),left(std::move(left)),right(std::move(right)) {}
};

struct CallExpr : Expr {
    std::string callee;
    std::unique_ptr<Expr> argument;

    CallExpr(const std::string &c, std::unique_ptr<Expr> arg)
        : callee(c), argument(std::move(arg)) {}
};

//=======================================================

struct Stmt {
    virtual ~Stmt() = default;
};

struct PrintStmt : Stmt {
    std::unique_ptr<Expr> expression;

    PrintStmt(std::unique_ptr<Expr> expression)
        : expression(std::move(expression)) {}
};

struct BreakStmt : Stmt{
    BreakStmt() = default;
};
struct InputStmt : Stmt {
    std::string name;

    InputStmt(const std::string &name)
        : name(name) {}
};

struct AssignStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> expression;

    AssignStmt(const std::string &n, std::unique_ptr<Expr> e)
        : name(n), expression(std::move(e)) {}
};

// Block statement: { stmt; stmt; ... }
struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;

    BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts)
        : statements(std::move(stmts)) {}
};

// If statement
struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Stmt>> thenBody;
    std::vector<std::unique_ptr<Stmt>> elseBody;

    IfStmt(
        std::unique_ptr<Expr> cond,std::vector<std::unique_ptr<Stmt>> thenB, std::vector<std::unique_ptr<Stmt>> elseB)
        : condition(std::move(cond)),thenBody(std::move(thenB)),elseBody(std::move(elseB)) {}
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::vector<std::unique_ptr<Stmt>> body;

    WhileStmt(
        std::unique_ptr<Expr> cond,
        std::vector<std::unique_ptr<Stmt>> body
    )
        : condition(std::move(cond)),
          body(std::move(body)) {}
};
