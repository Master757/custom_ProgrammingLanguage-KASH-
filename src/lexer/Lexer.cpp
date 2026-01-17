#include "Lexer.h"
#include <cctype>

using namespace std;

Lexer::Lexer(const std::string &s): s(s), position(0) {}

// peep current character without consuming
char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return s[position];
}

// consume and return current character
char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char toRet = s[position];
    position++;
    return toRet;
}

bool Lexer::isAtEnd() const {
    return position >= s.length();
}

void Lexer::skipSpace() {
    while (!isAtEnd() && std::isspace(static_cast<unsigned char>(peek()))) {
        advance();
    }
}

Token Lexer::makeNumber() {
    std::string val;
    bool hasDot = false;

    while (!isAtEnd()) {
        char c = peek();

        if (std::isdigit(static_cast<unsigned char>(c))) {
            val += advance();
        }
        else if (c == '.' && !hasDot) {
            hasDot = true;
            val += advance();
        }
        else {
            break;
        }
    }

    if (hasDot)
        return { TokenTypes::FLOAT, val };
    else
        return { TokenTypes::NUMBER, val };
}

Token Lexer::makeIdentifier() {
    std::string val;
    while (!isAtEnd() && (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')) {
        val += advance();
    }

    //different Token types accoring to their values {assignment}
    if (val == "out")  return { TokenTypes::OUT, val };
    if (val == "in")   return { TokenTypes::IN,  val };
    if (val == "if")   return { TokenTypes::IF,  val };     
    if (val == "else") return { TokenTypes::ELSE, val };   
    if (val == "while") return { TokenTypes::WHILE, val };
    if (val == "break") return {TokenTypes::BREAK, val};

    return { TokenTypes::IDENTIFIER, val };
}

void Lexer::commenting(){
    advance();
    while(!isAtEnd() && peek() != '#' && peek() != '\n'){
        advance();
    }

    if (!isAtEnd() && peek() == '#') {
        advance();
    }


}

Token Lexer::makeString() {
    advance(); // skip opening quote
    std::string val;
    while (!isAtEnd() && peek() != '"') {
        val += advance();
    }

    if (!isAtEnd() && peek() == '"') {
        advance();
    }

    return { TokenTypes::STRING, val };
}


std::vector<Token> Lexer::tokenize() {
    std::vector<Token> t;

    while (!isAtEnd()) {
        skipSpace();
        if (isAtEnd()) break;

        char c = peek();

        if (std::isdigit(static_cast<unsigned char>(c))) {
            t.push_back(makeNumber());
        }
        else if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            t.push_back(makeIdentifier());
        }
        else {
            switch (c) {
                case '(':
                    advance();
                    t.push_back({ TokenTypes::PAREN_L, "(" });
                    break;
                case ')':
                    advance();
                    t.push_back({ TokenTypes::PAREN_R, ")" });
                    break;
                case ';':
                    advance();
                    t.push_back({ TokenTypes::SEMICOLON, ";" });
                    break;
                case '+':
                    advance();
                    t.push_back({ TokenTypes::PLUS, "+" });
                    break;
                case '-':
                    advance();
                    t.push_back({ TokenTypes::MINUS, "-" });
                    break;
                case '*':
                    advance();
                    t.push_back({ TokenTypes::ASTERISK, "*" });
                    break;
                case '/':
                    advance();
                    t.push_back({ TokenTypes::SLASH, "/" });
                    break;
                case '%':
                    advance();
                    t.push_back({ TokenTypes::MODULUS, "%" });
                    break;
                case '"':
                    t.push_back(makeString());
                    break;
                case '#':
                    commenting();
                    break;
                case '=':
                    advance();
                    if (!isAtEnd() &&peek() == '='){//comparision sign checked
                        t.push_back( {TokenTypes::EQUAL_EQUAL, "=="});
                        advance();
                    }else{
                    t.push_back({ TokenTypes::EQUALS, "=" });}
                    break;
                case '{':
                    advance();
                    t.push_back({ TokenTypes::CURLY_L, "{" });
                    break;

                case '}':
                    advance();
                    t.push_back({ TokenTypes::CURLY_R, "}" });
                    break;

                case '!':
                    advance();
                    if(!isAtEnd() && peek() == '='){//comparision sign checked
                        t.push_back({ TokenTypes::NOT_EQUAL, "!=" });
                        advance();
                    }else{
                        //urinary operator later
                    }
                    break;
                case '<':
                    advance();
                    if(!isAtEnd() && peek() == '='){
                        advance(); 
                        t.push_back({ TokenTypes::LESSER_EQUAL, "<=" });
                    }else{ t.push_back({ TokenTypes::LESSER, "<" });}
                    break;

                case '>':
                    advance();
                    if(!isAtEnd() && peek() == '='){
                        advance(); 
                        t.push_back({ TokenTypes::GREATER_EQUAL, ">=" });
                    }else{ t.push_back({ TokenTypes::GREATER, ">" });}
                    break;
                default:
                    // unknown char: give error
                    advance();
                    break;
            }
        }
    }

    t.push_back({ TokenTypes::END_OF_FILE, "" });
    return t;
}
