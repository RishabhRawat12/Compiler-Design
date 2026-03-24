# 🚀 Static Code Analyzer with Error Handling

A web-based Static Code Analyzer designed to help students and learners understand how code is compiled and where errors occur. This project demonstrates core concepts of compiler design including lexical, syntax, and semantic analysis.

---

## 📌 Features

- 🧩 Lexical Analysis (Token Generation)
- 🌳 Syntax Analysis (Parse Tree / AST)
- 🧠 Semantic Analysis (Symbol Table & Type Checking)
- ❌ Clear and Human-Readable Error Messages
- 🖥️ Web-Based Code Editor
- 🔐 Secure Code Execution Environment
- 💾 Save Code & Execution History
- 👤 User Authentication System

---

## 🏗️ Project Structure
Static-Code-Analyzer/
│
├── frontend/ # UI (HTML, CSS, JS)
├── backend/ # Server-side logic
├── lexer/ # Lexical Analyzer
├── parser/ # Syntax Analyzer
├── semantic/ # Semantic Analyzer
├── database/ # DB Config & Models
└── README.md


---

## ⚙️ Working

1. User writes code in the web editor.
2. Code is passed to the Lexical Analyzer → Tokens are generated.
3. Tokens are parsed by Syntax Analyzer → AST is built.
4. Semantic Analyzer checks for logical errors.
5. Errors (if any) are displayed with line numbers and explanations.
6. Code is executed in a secure environment.
7. Results and history are stored in the database.

---

## 🧠 Compiler Phases Implemented

### 1. Lexical Analysis
- Converts source code into tokens
- Identifies keywords, identifiers, operators

### 2. Syntax Analysis
- Uses Recursive Descent Parser (LL(1))
- Builds Abstract Syntax Tree (AST)

### 3. Semantic Analysis
- Symbol Table Management
- Type Checking
- Detects:
  - Undeclared Variables
  - Redeclarations
  - Type Errors

---

## 🎯 Goals

- Help students understand compiler design practically
- Provide clear explanation of errors
- Build a modular and scalable analyzer system
- Bridge theory and real-world implementation

---

## 🚧 Challenges

- Designing LL(1) grammar
- Handling syntax and semantic errors efficiently
- Building and visualizing AST
- Ensuring secure execution environment

---

## 🔮 Future Scope

- Add Code Optimization Phase
- Support Multiple Programming Languages
- Improve Error Suggestions using AI
- Add Intermediate Code Generation

---

## 🛠️ Tech Stack

- Frontend: HTML, CSS, JavaScript
- Backend: (Node.js / Python - specify yours)
- Database: (MongoDB / MySQL - specify yours)

---

## ▶️ How to Run

```bash
# Clone the repository
git clone https://github.com/your-username/static-code-analyzer.git

# Navigate to project folder
cd static-code-analyzer

# Install dependencies

# Run the project
