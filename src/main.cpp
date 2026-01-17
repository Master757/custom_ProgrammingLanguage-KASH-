#include <iostream>
#include <fstream>
#include <sstream>

#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "interpreter/Interpreter.h"

int main() {
    // Open source file
    std::ifstream file("examples/test.myc");
    if (!file) {
        std::cerr << "Error: could not open examples/test.myc\n";
        return 1;
    }

    // Read entire file into a string
    std::stringstream buffer;
    buffer << file.rdbuf();

    // ===== Lexing =====
    Lexer lexer(buffer.str());
    auto tokens = lexer.tokenize();

    // ===== Parsing =====
    Parser parser(tokens);
    auto program = parser.parse();

    // ===== Interpreting =====
    Interpreter interpreter;
    interpreter.interpret(program);

    return 0;
}
