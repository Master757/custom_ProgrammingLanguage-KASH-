THIS IS A PROJECT STILL ONGOING

# Kash Programming Language — v1.0

Kash is a small, interpreted programming language written in **C++**.  
It is designed as a learning-focused language to explore how real programming languages work internally — from lexing and parsing to runtime execution.

This repository represents **Version 1.0**, where Kash has grown into a fully working interpreted language with variables, control flow, expressions, and dynamic typing.

---

##  Features (v1.0)

### Core Language
- Dynamic typing (numbers, floats, strings)
- Variables and assignments
- Arithmetic operations (`+ - * / %`)
- Comparison operators (`== != < <= > >=`)
- String concatenation
- Built-in input and output

### Control Flow
- `if / else` statements
- `while` loops
- `break` statements
- Block scoping using `{ }`

### Built-in Functions
- `toNum(x)` – convert string to number
- `toString(x)` – convert number to string

### Runtime
- Interpreted execution (no bytecode or compilation)
- Variant-based runtime values
- Exception-based control flow for `break`

---

##  Language Design Philosophy

- **Simple syntax**, inspired by C / Python
- **Explicit structure**, but flexible typing
- **Readable internals**, prioritizing learning over performance
- Designed to be **extended gradually** (functions, recursion, GUI, etc.)

---

## 🗂 Project Structure

Kash/
├── examples/ # Example .myc programs
│ └── test.myc
│
├── src/
│ ├── lexer/ # Tokenization
│ │ ├── Lexer.h
│ │ ├── Lexer.cpp
│ │ └── Token.h
│ │
│ ├── parser/ # Parsing + AST
│ │ ├── Parser.h
│ │ ├── Parser.cpp
│ │ └── AST.h
│ │
│ ├── interpreter/ # Runtime execution
│ │ ├── Interpreter.h
│ │ └── Interpreter.cpp
│ │
│ └── main.cpp # Entry point
│
├── runtime/ # Reserved for future runtime features
├── gui/ # Planned GUI frontend
└── README.md



---

## How Kash Works (High-Level)

### 1️⃣ Lexing
Source code is converted into a sequence of tokens  
(`IDENTIFIER`, `NUMBER`, `STRING`, `IF`, `WHILE`, operators, etc.)

### 2️⃣ Parsing
Tokens are transformed into an **Abstract Syntax Tree (AST)**  
The AST represents program structure, not execution.

### 3️⃣ Interpreting
The interpreter walks the AST:
- **Statements** are executed (`if`, `while`, `print`, `assign`)
- **Expressions** are evaluated to runtime values
- Runtime values are stored in an environment (`env`)

Control flow (`break`, loops) is handled internally using structured execution and signals.

---

## 🧪 Example Program

```kash
in(n);
n = toNum(n);

i = 0;
a = 0;
b = 1;

while (i < n) {
    temp = b;
    b = a + b;
    a = temp;
    i = i + 1;
}

out(a);

```

**compile : g++ -std=c++17 src/main.cpp src/lexer/Lexer.cpp src/parser/Parser.cpp src/interpreter/Interpreter.cpp -o kash


**run : ./kash examples/test.myc

**Project Goal**

The goal of Kash is not to replace existing languages, but to:

Deeply understand how programming languages are built

Experiment safely with language features


Serve as a foundation for further compiler / interpreter work
