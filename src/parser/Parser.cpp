#include "Parser.h"
#include <stdexcept>
#include <iostream>

//consturctor
Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), cur(0) {}



const Token& Parser::peek() const {
    return tokens[cur];
}

const Token& Parser::previous() const {
    return tokens[cur - 1];
}

bool Parser::isAtEnd() const {
    return peek().t == TokenTypes::END_OF_FILE;
}

const Token& Parser::advance() {
    if (!isAtEnd()) cur++;
    return previous();
}

bool Parser::check(TokenTypes type) const {
    if (isAtEnd()) return false;
    return peek().t == type;
}

bool Parser::match(TokenTypes type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!isAtEnd()) {
        if(check(TokenTypes::END_OF_FILE)) {break;};
        statements.push_back(parseStatement());
    }

    return statements;
}


// Parses a block: assumes current token is '{' (it will consume it).
// Returns a vector of statements that were inside the block.
std::vector<std::unique_ptr<Stmt>> Parser::parseBlock() {
    if (!match(TokenTypes::CURLY_L))
        throw std::runtime_error("Expected '{' to start block");

    std::vector<std::unique_ptr<Stmt>> stmts;

    while (!check(TokenTypes::CURLY_R) && !isAtEnd()) {
        stmts.push_back(parseStatement());
    }

    if (!match(TokenTypes::CURLY_R))
        throw std::runtime_error("Expected '}' to close block");

    return stmts;
}


std::unique_ptr<Stmt> Parser::parseStatement() {
    // Skip leading comments
    while (check(TokenTypes::COMMENT)) advance();

    // block as a statement: { ... }
    if (check(TokenTypes::CURLY_L)) {
        auto body = parseBlock();
        return std::make_unique<BlockStmt>(std::move(body));
    }
    
    if (match(TokenTypes::BREAK)) {
        if (loopDepth == 0) {
            throw std::runtime_error("'break' used outside of a loop");
        }

        if (!match(TokenTypes::SEMICOLON))
            throw std::runtime_error("Expected ';' after 'break'");

        return std::make_unique<BreakStmt>();
    }


    // out(expression);
    if (match(TokenTypes::OUT)) {
        if (!match(TokenTypes::PAREN_L))
            throw std::runtime_error("Expected '(' after 'out'");

        auto expr = parseExpression();

        if (!match(TokenTypes::PAREN_R))
            throw std::runtime_error("Expected ')' after expression");

        if (!match(TokenTypes::SEMICOLON))
            throw std::runtime_error("Expected ';' after out statement");

        return std::make_unique<PrintStmt>(std::move(expr));
    }

    // in(identifier);
    if (match(TokenTypes::IN)) {
        if (!match(TokenTypes::PAREN_L))
            throw std::runtime_error("Expected '(' after 'in'");

        if (!match(TokenTypes::IDENTIFIER))
            throw std::runtime_error("Expected identifier in in()");

        std::string name = previous().value;

        if (!match(TokenTypes::PAREN_R))
            throw std::runtime_error("Expected ')' after identifier");

        if (!match(TokenTypes::SEMICOLON))
            throw std::runtime_error("Expected ';' after in statement");

        return std::make_unique<InputStmt>(name);
    }

    // if (condition) { ... }
    if (match(TokenTypes::IF)) {
        if (!match(TokenTypes::PAREN_L))
            throw std::runtime_error("Expected '(' after 'if'");

        auto condition = parseExpression();

        if (!match(TokenTypes::PAREN_R))
            throw std::runtime_error("Expected ')' after if condition");

        auto thenBody = parseBlock();

        std::vector<std::unique_ptr<Stmt>> elseBody;

        if (match(TokenTypes::ELSE)) {
            elseBody = parseBlock();
        }

        return std::make_unique<IfStmt>(
            std::move(condition),
            std::move(thenBody),
            std::move(elseBody)
        );
    }

    // while (condition) { ... }
if (match(TokenTypes::WHILE)) {
    if (!match(TokenTypes::PAREN_L))
        throw std::runtime_error("Expected '(' after 'while'");

    auto condition = parseExpression();

    if (!match(TokenTypes::PAREN_R))
        throw std::runtime_error("Expected ')' after while condition");

    loopDepth++;
    auto body = parseBlock();
    loopDepth--;

    return std::make_unique<WhileStmt>(
        std::move(condition),
        std::move(body)
    );
}

    // assignment: identifier = expression;
    if (check(TokenTypes::IDENTIFIER)) {
        // look ahead safely (skip comments)
        size_t i = cur + 1;
        while (i < tokens.size() && tokens[i].t == TokenTypes::COMMENT) i++;

        bool isAssign = (i < tokens.size() && tokens[i].t == TokenTypes::EQUALS);

        if (isAssign) {
            advance(); // consume identifier
            std::string name = previous().value;

            // consume any comments as they are ignored either way
            while (check(TokenTypes::COMMENT)) advance();

            // consume '=' {we dont want this to go for calculation}
            if (!match(TokenTypes::EQUALS))
                throw std::runtime_error("Expected '=' after identifier");

            auto expr = parseExpression();

            if (!match(TokenTypes::SEMICOLON))
                throw std::runtime_error("Expected ';' after assignment");

            return std::make_unique<AssignStmt>(name, std::move(expr));
        }
    }

    throw std::runtime_error("Unknown statement");
}


std::unique_ptr<Expr> Parser::parseExpression() {
    return parseComparison();
}

std::unique_ptr<Expr> Parser::parseComparison() {
    auto expr = parseTerm();

    while (
        match(TokenTypes::EQUAL_EQUAL) ||
        match(TokenTypes::NOT_EQUAL) ||
        match(TokenTypes::GREATER) ||
        match(TokenTypes::LESSER) ||
        match(TokenTypes::GREATER_EQUAL) ||
        match(TokenTypes::LESSER_EQUAL)
    ) {
        TokenTypes op = previous().t;
        auto right = parseTerm();

        expr = std::make_unique<BinaryExpr>(
            op,
            std::move(expr),
            std::move(right)
        );
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseTerm() {
    auto expr = parseFactor();

    while (match(TokenTypes::PLUS) || match(TokenTypes::MINUS)) {
        TokenTypes op = previous().t;
        auto right = parseFactor();

        expr = std::make_unique<BinaryExpr>(
            op,
            std::move(expr),
            std::move(right)
        );
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parseFactor() {
    auto expr = parsePrimary();

    while (
        match(TokenTypes::ASTERISK) ||
        match(TokenTypes::SLASH) ||
        match(TokenTypes::MODULUS)
    ) {
        TokenTypes op = previous().t;
        auto right = parsePrimary();

        expr = std::make_unique<BinaryExpr>(
            op,
            std::move(expr),
            std::move(right)
        );
    }

    return expr;
}

std::unique_ptr<Expr> Parser::parsePrimary() {

    if (match(TokenTypes::NUMBER)) {
    int v = std::stoi(previous().value);
    return std::make_unique< literalExpressions>(v);
        }

        if (match(TokenTypes::FLOAT)) {
            double v = std::stod(previous().value);
            return std::make_unique< literalExpressions>(v);
        }


    if (match(TokenTypes::STRING)) {
        return std::make_unique<StringExpr>(previous().value);
    }

    if (match(TokenTypes::IDENTIFIER)) {
        std::string name = previous().value;

        // function call
        if (match(TokenTypes::PAREN_L)) {
            auto arg = parseExpression();

            if (!match(TokenTypes::PAREN_R))
                throw std::runtime_error("Expected ')' after function argument");

            return std::make_unique<CallExpr>(name, std::move(arg));
        }

        return std::make_unique<VariableExpr>(name);
    }

    if (match(TokenTypes::PAREN_L)) {
        auto expr = parseExpression();

        if (!match(TokenTypes::PAREN_R))
            throw std::runtime_error("Expected ')' after expression");

        return expr;
    }

    throw std::runtime_error("Expected expression");
}
