#include "Parser.h"

// Initialize current token and peek token from the lexer
void parser_init(Parser* self, Lexer* lexer, Emitter* emitter)
{
    self->lexer = lexer;
    self->emitter = emitter;

    if (self->lexer == NULL)
    {
        parser_abort(self, "Failed to bind lexer.");
    }

    self->declaredVars = ht_create();
    self->declaredLabels = ht_create();
    self->gotoedLabels = ht_create();

    self->curToken = lexer_getToken(self->lexer);
    lexer_nextChar(self->lexer);
    self->peekToken = lexer_getToken(self->lexer);
    lexer_nextChar(self->lexer);

    printf("Initialized Parser: completed - Current Token: %s, Peek Token: %s\n", self->curToken.text, self->peekToken.text);
}

// Start parsing process
void parser_start(Parser* self)
{
    printf("----Start Program----\n");

    // Generate header in output file
    emitter_header(self->emitter, "#include <stdio.h>\n");
    emitter_header(self->emitter, "int main(){\n");

    // Handle new lines at the start of the file
    while (parser_checkToken(self, NEWLINE))
    {
        parser_nextToken(self);
    }

    // Parse all the statements in the program
    while (!parser_checkToken(self, _EOF_))
    {
        parser_statement(self);
    }

    // Wrap the code.
    emitter_emit(self->emitter, "return 0;\n");
    emitter_emit(self->emitter, "}\n");

    // Check that GOTO labels are all declared
    hti itr_goto = ht_iterator(self->gotoedLabels);
    while (ht_next(&itr_goto))
    {
        void* value = ht_get(self->declaredLabels, (char*)itr_goto.value);
        if (value == NULL)
        {
            char str[] = "Attempting to GOTO to undeclared label: ";
            size_t len = strlen(str) + strlen(self->curToken.text);
            char* message = (char*)malloc(len + 1);
            strcpy(message, str);
            strcat(message, self->curToken.text);
            message[len] = '\0';
            parser_abort(self, message);
        }
    }

    printf("Parsing complete.\n");
}

// Process through each token
void parser_statement(Parser* self)
{
    if (parser_checkToken(self, PRINT))
    {
        // PRINT <string>
        printf("STATEMENT-PRINT\n");
        parser_nextToken(self);

        if (parser_checkToken(self, STRING))
        {
            printf("STRING: %s\n", self->curToken.text);
            
            // Generate printf with string line
            emitter_emit(self->emitter, "printf(\"");
            emitter_emit(self->emitter, self->curToken.text);
            emitter_emit(self->emitter, "\\n\");\n");
            parser_nextToken(self);
        }
        else
        {
            // Generate printf with number line
            emitter_emit(self->emitter, "printf(\"%%.2f\\n\",(float)(");

            parser_expression(self);

            emitter_emit(self->emitter, "));\n");
        }
    }
    else if (parser_checkToken(self, IF))
    {
        // IF <comparison> THEN
        // (any statement...)
        // ENDIF
        printf("STATEMENT-IF\n");
        parser_nextToken(self);
        emitter_emit(self->emitter, "if (");

        parser_comparison(self);

        parser_match(self, THEN);
        parser_nl(self);
        emitter_emit(self->emitter, ") {\n");

        while (!parser_checkToken(self, ENDIF))
        {
            parser_statement(self);
        }

        parser_match(self, ENDIF);
        emitter_emit(self->emitter, "}\n");
    }
    else if (parser_checkToken(self, WHILE))
    {
        // WHILE <comparison> REPEAT
        // (any statement...)
        // ENDWHILE
        printf("STATEMENT-WHILE\n");
        parser_nextToken(self);
        emitter_emit(self->emitter, "while (");
        
        parser_comparison(self);

        parser_match(self, REPEAT);
        parser_nl(self);
        emitter_emit(self->emitter, ") {\n");

        while (!parser_checkToken(self, ENDWHILE))
        {
            parser_statement(self);
        }

        parser_match(self, ENDWHILE);
        emitter_emit(self->emitter, "}\n");
    }
    else if (parser_checkToken(self, LABEL))
    {
        // LABEL <identity>
        printf("STATEMENT-LABEL\n");
        parser_nextToken(self);

        // Check if this label already exists.
        void* value = ht_get(self->declaredLabels, self->curToken.text);
        // label exists in the declared list
        if (value != NULL)
        {
            char* str = "Label already exists: ";
            size_t len = strlen(str) + strlen(self->curToken.text);
            char* message = (char*)malloc(len + 1);
            strcpy(message, str);
            strcat(message, self->curToken.text);
            message[len] = '\0';
            parser_abort(self, message);
        }
        else
        {
            ht_set(self->declaredLabels, self->curToken.text, self->curToken.text);
        }
        
        parser_match(self, IDENT);

        // Emit label and :
        emitter_emit(self->emitter, self->curToken.text);
        emitter_emit(self->emitter, ":\n");
    }
    else if (parser_checkToken(self, GOTO))
    {
        // GOTO <identity>
        printf("STATEMENT-GOTO\n");
        parser_nextToken(self);

        // Check if this label already exists.
        void* value = ht_get(self->gotoedLabels, self->curToken.text);
        // add if not exist in gotoedLabel
        if (value == NULL)
        {
            ht_set(self->gotoedLabels, self->curToken.text, self->curToken.text);
        }

        parser_match(self, IDENT);

        // Space of goto, colon tokenText, and \0
        emitter_emit(self->emitter, "goto ");
        emitter_emit(self->emitter, self->curToken.text);
        emitter_emit(self->emitter, ";");
    }
    else if (parser_checkToken(self, LET))
    {
        // LET <identity> = <expression>
        printf("STATEMENT-LET\n");
        parser_nextToken(self);

        void* value = ht_get(self->declaredVars, self->curToken.text);

        // variable is not declared yet
        if (value == NULL)
        {
            ht_set(self->declaredVars, self->curToken.text, self->curToken.text);

            emitter_header(self->emitter, "float ");
            emitter_header(self->emitter, self->curToken.text);
            emitter_header(self->emitter, ";\n");
        }

        emitter_emit(self->emitter, self->curToken.text);
        emitter_emit(self->emitter, " = ");
        parser_match(self, IDENT);
        parser_match(self, EQ);
        parser_expression(self);
        emitter_emit(self->emitter, ";\n");
    }
    else if (parser_checkToken(self, INPUT))
    {
        // INPUT <identity>
        printf("STATEMENT-LABEL\n");
        parser_nextToken(self);

        void* value = ht_get(self->declaredVars, self->curToken.text);
        if (value == NULL)
        {
            ht_set(self->declaredVars, self->curToken.text, self->curToken.text);
            emitter_header(self->emitter, "float ");
            emitter_header(self->emitter, self->curToken.text);
            emitter_header(self->emitter, ";\n");
        }

        // Scanf
        emitter_emit(self->emitter, "scanf(\"%%f\", &");
        emitter_emit(self->emitter, self->curToken.text);
        emitter_emit(self->emitter, ");\n");
        parser_match(self, IDENT);
    }
    else
    {
        char str1[] = "Invalid statement at ";
        char str2[] = ", Type: ";
        char scurTokenType[5];
        itoa(self->curToken.type, scurTokenType, 5);
        char* message = (char*)malloc(strlen(str1) + strlen(self->curToken.text) + strlen(str2) + strlen(scurTokenType) + 1);
        strcpy(message, str1);
        strcat(message, self->curToken.text);
        strcat(message, str2);
        strcat(message, scurTokenType);
        message[strlen(str1) + strlen(self->curToken.text) + strlen(str2) + strlen(scurTokenType)] = '\0';
        parser_abort(self, message);
    }

    parser_nl(self);
}

void parser_nl(Parser* self)
{
    printf("NEWLINE\n");
    parser_match(self, NEWLINE);

    while (parser_checkToken(self, NEWLINE))
    {
        parser_nextToken(self);
    }
}

// comparison ::= expression ((== | != | > | >= | < | <=) expression)+
void parser_comparison(Parser* self)
{
    printf("COMPARISON\n");

    parser_expression(self);

    if (parser_isComparisonOp(self))
    {
        emitter_emit(self->emitter, self->curToken.text);
        parser_nextToken(self);
        parser_expression(self);
    }
    else
    {
        char str[] = "Expected comparison operator at : ";
        size_t len = strlen(str) + strlen(self->curToken.text);
        char* message = malloc(len + 1);
        strcpy(message, str);
        strcat(message, self->curToken.text);
        message[len] = '\0';
        parser_abort(self, message);
    }

    while (parser_isComparisonOp(self))
    {
        emitter_emit(self->emitter, self->curToken.text);
        parser_nextToken(self);
        parser_expression(self);
    }
}

// expression ::= term {(- | +) term}
void parser_expression(Parser* self)
{
    printf("EXPRESSION\n");

    parser_term(self);

    while (parser_checkToken(self, PLUS) || parser_checkToken(self, MINUS))
    {
        emitter_emit(self->emitter, self->curToken.text);
        parser_nextToken(self);
        parser_term(self);
    }
}

// term ::= unary {(* | /) unary}
void parser_term(Parser* self)
{
    printf("TERM\n");

    parser_unary(self);

    while (parser_checkToken(self, ASTERISK) || parser_checkToken(self, SLASH))
    {
        emitter_emit(self->emitter, self->curToken.text);
        parser_nextToken(self);
        parser_unary(self);
    }
}

// unary ::= {(+ | -)} primary
void parser_unary(Parser* self)
{
    printf("UNARY\n");

    if (parser_checkToken(self, PLUS) || parser_checkToken(self, MINUS))
    {
        emitter_emit(self->emitter, self->curToken.text);
        parser_nextToken(self);
    }
    else if (!(parser_checkToken(self, NUMBER) || parser_checkToken(self, IDENT)))
    {
        char str[] = "Expected in front of unary to be + or - at : ";
        size_t len = strlen(str) + strlen(self->curToken.text);
        char* message = malloc(len + 1);
        strcpy(message, str);
        strcat(message, self->curToken.text);
        message[len] = '\0';
        parser_abort(self, message);
    }

    parser_primary(self);
}

// primary ::= NUMBER | IDENT
void parser_primary(Parser* self)
{
    printf("PRIMARY: %s\n", self->curToken.text);

    if (parser_checkToken(self, NUMBER))
    {
        emitter_emit(self->emitter, self->curToken.text);
        parser_nextToken(self);
    }
    else if (parser_checkToken(self, IDENT))
    {
        void* value = ht_get(self->declaredVars, self->curToken.text);
        if (value == NULL)
        {
            char str[] = "Referencing variable before declaration: ";
            size_t len = strlen(str) + strlen(self->curToken.text);
            char* message = (char*)malloc(len + 1);
            strcpy(message, str);
            strcat(message, self->curToken.text);
            message[len] = '\0';
            parser_abort(self, message);
        }

        emitter_emit(self->emitter, self->curToken.text);
        parser_nextToken(self);
    }
    else
    {
        char str[] = "Expected primary to be either number or identity at :";
        size_t len = strlen(str) + strlen(self->curToken.text);
        char* message = malloc(len + 1);
        strcpy(message, str);
        strcat(message, self->curToken.text);
        message[len] = '\0';
        parser_abort(self, message);
    }
}

bool parser_isComparisonOp(Parser* self)
{
    return parser_checkToken(self, EQEQ) ||
        parser_checkToken(self, NOTEQ) ||
        parser_checkToken(self, GT) ||
        parser_checkToken(self, GTEQ) ||
        parser_checkToken(self, LT) ||
        parser_checkToken(self, LTEQ);
}

bool parser_checkToken(Parser* self, enum TokenType tokenType)
{
    return tokenType == self->curToken.type;
}

bool parser_checkPeek(Parser* self, enum TokenType tokenType)
{
    return tokenType == self->peekToken.type;
}

void parser_match(Parser* self, enum TokenType tokenType)
{
    if (!parser_checkToken(self, tokenType))
    {
        char str1[] = "Expected ";
        char str2[] = ", got ";
        char smatchType[5];
        char scurTokenType[5];
        itoa(tokenType, smatchType, 10);
        itoa(self->curToken.type, scurTokenType, 10);
        char* message = (char*)malloc(strlen(str1) + strlen(smatchType) + strlen(str2) + strlen(scurTokenType) + 1);
        strcpy(message, str1);
        strcat(message, smatchType);
        strcat(message, str2);
        strcat(message, scurTokenType);
        message[strlen(str1) + strlen(smatchType) + strlen(str2) + strlen(scurTokenType)] = '\0';
        parser_abort(self, message);
    }

    parser_nextToken(self);
}

void parser_nextToken(Parser* self)
{
    self->curToken = self->peekToken;
    lexer_nextChar(self->lexer);
    self->peekToken = lexer_getToken(self->lexer);
    // printf("Move to next token - Current token: %s, Peek token: %s\n", self->curToken.text, self->peekToken.text);
}

void parser_abort(Parser* self, char* message)
{
    printf("Parser error: %s", message);
    abort();
}