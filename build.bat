@echo off
echo Compiling the compiler...
g++ -std=c++17 main.cpp src/lexical/lexer.cpp src/syntax/parser.cpp src/semantics/semantic_analyzer.cpp src/AST/ast_printer.cpp src/icg/icg_generator.cpp src/optimizer/optimizer.cpp -Iinclude -o my_compiler.exe

if %errorlevel% equ 0 (
    echo Build Successful!
) else (
    echo Build Failed. Check the errors above.
)