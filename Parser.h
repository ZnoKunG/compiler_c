#ifndef PARSER_H
#define PARSER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Lexer.h"
#include "Emitter.h"
#include <ctype.h>
#include <stdbool.h>
#include "ht.c"

typedef struct Parser {
    Lexer* lexer;
    Emitter* emitter;
    Token curToken;
    Token peekToken;
    ht* declaredVars;
    ht* declaredLabels;
    ht* gotoedLabels;
} Parser;

void parser_init(Parser* self, Lexer* lexer, Emitter* emitter);
void parser_start(Parser* self);
void parser_statement(Parser* self);
bool parser_checkToken(Parser* self, enum TokenType tokenType);
bool parser_checkPeek(Parser* self, enum TokenType tokenType);
void parser_match(Parser* self, enum TokenType tokenType);
void parser_nextToken(Parser* self);
void parser_abort(Parser* self, char* message);

void parser_nl(Parser* self);
void parser_expression(Parser* self);
void parser_term(Parser* self);
void parser_unary(Parser* self);
void parser_primary(Parser* self);
void parser_comparison(Parser* self);
bool parser_isComparisonOp(Parser* self);

#endif