# Frontend Phases of Compiler

A modular and optimized C-like compiler implementation developed as part of a **Project-Based Learning (PBL)** curriculum. The project covers the complete journey from raw source code to optimized **Three-Address Code (TAC)**.

## 🚀 Project Overview

This compiler implements the first four critical phases of the compilation process. It is designed for a C-like high-level language, supporting basic data types, complex arithmetic expressions, conditional branching, loops, and function definitions.

### Key Features
- **Recursive Descent Parser**: A robust manual parser that handles operator precedence and nested structures.
- **Semantic Intelligence**: Advanced type-checking and scope validation, with warnings for unused variables.
- **Three-Address Code (TAC)**: Translates high-level code into a simplified instruction set.
- **Peephole Optimization**: A multi-pass optimizer that streamlines the generated intermediate code.

---

## 🏗️ The 4-Phase Pipeline

### Phase 1: Lexical Analysis
- **Module**: `src/lexical`
- Converts the source stream into a sequence of meaningful **Tokens**.
- Handles keywords (`int`, `if`, `for`), operators, literals, and multi-line comments.

### Phase 2: Syntax Analysis
- **Module**: `src/syntax` & `src/AST`
- Parses tokens into an **Abstract Syntax Tree (AST)**.
- Outputs the AST as a hierarchical **JSON** file for easy visualization.

### Phase 3: Semantic Analysis
- **Module**: `src/semantics`
- Performs mandatory safety checks:
    - **Type Checking**: Ensures compatibility in assignments and operations.
    - **Scope Management**: Validates global and local variable accessibility using a Symbol Table.
    - **Static Analysis**: Detects uninitialized variables and non-constant global initializers.

### Phase 4: Intermediate Code & Optimization
- **Module**: `src/icg` & `src/optimizer`
- **ICG**: Flattens the AST into **Three-Address Code (TAC)** instructions.
- **Optimizer**: Applies four key rules to reduce instructions and improve efficiency:
    - **Constant Folding**: `10 + 20` $\rightarrow$ `30`
    - **Algebraic Simplification**: Removes `+ 0`, `* 1`, and `* 0` logic.
    - **Copy Propagation**: Streamlines temporary variable assignments.
    - **Dead Code Elimination**: Removes unused temporary instructions.

---

## 📂 Project Structure

```text
├── src/
│   ├── lexical/     # Lexer & Token logic
│   ├── syntax/      # Parser logic
│   ├── AST/         # AST Node definitions
│   ├── semantics/   # Semantic Analyzer & Symbol Table
│   ├── icg/         # TAC Generator
│   └── optimizer/   # Peephole Optimizer
├── include/         # External headers (json.hpp)
├── output/          # Compiler results (JSON, TXT)
├── build.bat        # Compilation script
└── main.cpp         # Main entry point - Orchestrates the pipeline
```

---

## 🛠️ Getting Started

### Prerequisites
- A C++ compiler supporting the **C++17** standard (e.g., MinGW/GCC).

### Building the Project
Run the provided batch script to compile the project into `my_compiler.exe`:
```powershell
.\build.bat
```

### Running the Compiler
To process a source file (e.g., `p.txt`), use the following command:
```powershell
.\my_compiler.exe <input_file> <tokens.json> <ast.json> <semantics.json> [tac_output.txt]
```

**Example:**
```powershell
.\my_compiler.exe test_opt.txt output/tokens.json output/ast.json output/sem.json output/intermediate.txt
```

---

## 📝 Example Output (Optimization)

The compiler outputs both the raw and optimized TAC to allow for performance comparison.

**Source Code:**
```cpp
int a = 10 + 20;
int b = a + 0;
```

**Optimized TAC:**
```text
a = 30
b = a
```
*Result: Automatically folded constants and removed identity operations.*
