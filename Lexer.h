#ifndef LEXER_H
#define LEXER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Token.c"
#include <ctype.h>

typedef struct Lexer {
    char* source;
    char curChar;
    int curPos;
} Lexer;

void lexer_init(Lexer* self, const char* source);
void lexer_nextChar(Lexer* self);
char lexer_peek(Lexer* self);
void lexer_abort(Lexer* self, char* message);
void lexer_skipWhiteSpace(Lexer* self);
void lexer_skipComment(Lexer* self);
Token lexer_getToken(Lexer* self);

#endif