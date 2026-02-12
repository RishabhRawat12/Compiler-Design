# _EduLex_ Programming Language

EduLex is a minimal and structured programming language designed for learning lexical analysis and compiler construction.

It emphasizes strict rules, readable syntax, and simplicity.

---

# Reserved Keywords

The following words are reserved and cannot be used as identifiers:

num  
text  
phrase  
when  
otherwise  
cycle  
project  
hollow  
skip  
shatter  

---

# Data Types

## 1. num

Represents numeric values (integers and floating-point numbers).

Rules:
- Supports integers and decimals
- Scientific notation is NOT supported
- Must contain digits before decimal point
- `.5` is invalid
- `10.` is invalid

Example:
num x = 10
num price = 99.5
num ratio = 0.75

---

## 2. text

Represents a single character.

Rules:
- Must be enclosed in single quotes
- Must contain exactly one character
- Escape sequences supported: '\n', '\t', '\'', '\\'
- `'AB'` is invalid
- `''` is invalid

Example:
text grade = 'A'
text symbol = '#'
text newline = '\n'


---

## 3. phrase

Represents a sequence of characters (string).

Rules:
- Must be enclosed in double quotes
- Escape sequences supported: "\n", "\t", "\"", "\\"
- Multi-line phrases are NOT allowed
- Empty phrase allowed: ""

Example:
phrase name = "EduLex"
phrase message = "Hello World"
phrase empty = ""


---

# Special Value

## hollow

Represents absence of value (null).

Example:

---

# Identifiers

Rules:
- Must start with a letter (A–Z or a–z)
- Can contain letters, digits, and underscore
- Case sensitive
- Cannot match any reserved keyword

Valid Examples:
value
total_sum
x1
DataPoint

Invalid Examples:
1value
num
when


---

# Operators

## Arithmetic Operators

+   Addition  
-   Subtraction  
*   Multiplication  
/   Division  
%   Modulus  

Example:
num total = 10 + 5 * 2

## Assignment Operator

=  

Example:
num x = 10

## Comparison Operators

==   Equal to  
!=   Not equal to  
>    Greater than  
<    Less than  
>=   Greater than or equal  
<=   Less than or equal  

Example:
when x >= 10 {
project "Large"
}

## Logical Operators

and  
or  
not  

Example:
when x > 5 and x < 20 {
project "Valid"
}

---

# Control Structures

## Conditional Statement

Structure:
when condition {
statements
}
otherwise {
statements
}

Example:
when price > 50 {
project "Expensive"
}
otherwise {
project "Affordable"
}

---

# Output

## project

Used to display output.

Example:
project "Hello World"
project value

---


# Whitespace Rules

- Language is case sensitive
- Whitespace is ignored except as separator
- Blocks must use { }
- Indentation has no syntactic meaning

---
