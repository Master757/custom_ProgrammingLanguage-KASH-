#pragma once

#include <string>
#include <vector>

#include "Token.h"


class Lexer{
    public :
        Lexer(const std::string &s);
     
        std::vector<Token> tokenize(); //a vector of Tokens as define in Token.h file
    private:
        char peek() const;
        char advance();
        bool isAtEnd() const;

        void skipSpace();

        //creating values from the struct as needed later 
        Token makeNumber();
        Token makeIdentifier();
        Token makeString();
        void commenting();


        std::string s;
        size_t position;

};