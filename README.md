
# A Simple Compiler in C

A simple compiler written in C which can do the basic operation including mathematical operation, conditions, loop, and input.

## Language Components
- *PRINT*: print out message either variable or pure float
- *LET*: create variable or (if variable exists) assign value
- *IF*: conditional structure followed with *THEN*
- *WHILE*: conditional loop followed with *REPEAT*
- *GOTO*: move to the line
- *LABEL*: create label for *GOTO*
- *INPUT*: receive input from the user

## How does it work
This compiler composes of three parts including **lexer**, **parser**, and **emitter**.
- **Lexer:** Extract the source code into *token*, word containing the text and type of *token*, using *lexer analysis*.
- **Parser:** Check the sequence of *tokens* if it follows the *language grammar*.
- **Emitter:** Write out the code following the sequence in the *parser* into C code.

## References
This project follows this tutorial https://austinhenley.com/blog/teenytinycompiler1.html which is written in Python.

