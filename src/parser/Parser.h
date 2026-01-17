#pragma once
#include <vector>
#include <memory>

#include "../lexer/Token.h"
#include "AST.h"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parse();

private:
    int loopDepth = 0;

    const std::vector<Token>& tokens;
    size_t cur;

    // token helpers
    const Token& peek() const;
    const Token& previous() const;
    bool isAtEnd() const;
    const Token& advance();
    bool check(TokenTypes type) const;
    bool match(TokenTypes type);
    bool checkNext(TokenTypes type) const;


    std::unique_ptr<Stmt> parseStatement();
    std::vector<std::unique_ptr<Stmt>> parseBlock();


    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseComparison();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parseFactor();
    std::unique_ptr<Expr> parsePrimary();
};
