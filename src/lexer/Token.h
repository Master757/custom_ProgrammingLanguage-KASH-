
#pragma once
#include <string>

//contains all the keywords and operators {will add to this later on for development}
enum class TokenTypes{
    OUT,
    IN,
    IDENTIFIER,
    NUMBER,
    BREAK,
    STRING,
    FLOAT,
    WHILE,
    IF,
    ELSE,
    PAREN_L,
    PAREN_R,
    CURLY_L,
    CURLY_R,

    GREATER,
    LESSER,
    GREATER_EQUAL,
    LESSER_EQUAL,
    EQUALS,
    EQUAL_EQUAL,
    NOT_EQUAL,

    SEMICOLON,
    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    MODULUS,
    COMMENT,

    END_OF_FILE
};

struct Token{
    TokenTypes t;
    std::string value;
};