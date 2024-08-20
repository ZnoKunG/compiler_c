#include "Lexer.h"

void lexer_init(Lexer* self, const char* source)
{
    // Allocate memory (extra 2 for \0)
    self->source = (char*)malloc(strlen(source) + 2);

    if (self->source == NULL)
    {
        lexer_abort(self, "Failed to allocate memory for source string.");
    }

    strcpy(self->source, source);
    strcat(self->source, "\n");

    printf("Initialized Lexer: completed.\n");

    self->curChar = '\0';
    self->curPos = -1;
    lexer_nextChar(self);
}

void lexer_nextChar(Lexer* self)
{
    self->curPos += 1;
    if (self->curPos >= strlen(self->source))
    {
        self->curChar = '\0'; // End of the file
    }
    else
    {
        self->curChar = self->source[self->curPos];
    }
}

char lexer_peek(Lexer* self)
{
    if (self->curPos + 1 >= strlen(self->source))
    {
        return '\0';
    }
    else
    {
        return self->source[self->curPos + 1];
    }
}

void lexer_abort(Lexer* self, char* message)
{
    printf("Lexing error: %s", message);
    abort();
}

void lexer_skipWhiteSpace(Lexer* self)
{
    while (self->curChar == ' ' || self->curChar == '\t' || self->curChar == '\r')
    {
        lexer_nextChar(self);
    }
}

void lexer_skipComment(Lexer* self)
{
    if (self->curChar == '#')
    {
        printf(">>> Skip comment\n");
        while (self->curChar != '\n')
        {
            lexer_nextChar(self);
        }
    }
}

Token lexer_getToken(Lexer* self)
{
    lexer_skipWhiteSpace(self);
    lexer_skipComment(self);

    Token token;
    char* curTokenChar = malloc(2);
    curTokenChar[0] = self->curChar;
    curTokenChar[1] = '\0';

    if (self->curChar == '+')
    {
        token_init(&token, curTokenChar, PLUS);
    }
    else if (self->curChar == '-')
    {
        token_init(&token, curTokenChar, MINUS);
    }
    else if (self->curChar == '*')
    {
        token_init(&token, curTokenChar, ASTERISK);
    }
    else if (self->curChar == '/')
    {
        token_init(&token, curTokenChar, SLASH);
    }
    else if (self->curChar == '\n')
    {
        token_init(&token, "NEWLINE", NEWLINE);
    }
    else if (self->curChar == '\0')
    {
        token_init(&token, "EOF", _EOF_);
    }
    else if (isalpha(self->curChar))
    {
        int startPos = self->curPos;
        while (isalnum(lexer_peek(self)))
        {
            lexer_nextChar(self);
        }

        int len = self->curPos - startPos + 1;
        char* tokenText = (char*)malloc(len + 1);
        strncpy(tokenText, self->source + startPos, len);
        tokenText[len] = '\0';
        enum TokenType type = token_getKeyword(tokenText);
        token_init(&token, tokenText, type);
    }
    else if (isdigit(self->curChar))
    {
        int startPos = self->curPos;
        while (isdigit(lexer_peek(self)))
        {
            lexer_nextChar(self);
        }

        if (lexer_peek(self) == '.') // Decimal
        {
            lexer_nextChar(self);

            if (!isdigit(lexer_peek(self)))
            {
                lexer_abort(self, "Illegal character after decimal point!");
            }
            while (isdigit(lexer_peek(self)))
            {
                lexer_nextChar(self);
            }
        }

        int numberLen = self->curPos - startPos + 1;
        char* tokenText = (char*)malloc(numberLen + 1);
        strncpy(tokenText, self->source + startPos, numberLen);
        tokenText[numberLen] = '\0';

        token_init(&token, tokenText, NUMBER);
    }
    else if (self->curChar == '\"')
    {
        lexer_nextChar(self);
        int startPos = self->curPos;

        if (self->curChar == '\"')
        {
            token_init(&token, "", STRING);
        }
        else
        {
            while (self->curChar != '\"')
            {
                if (self->curChar == '\r' || self->curChar == '\n' || self->curChar == '\t' || self->curChar == '\\' || self->curChar == '%')
                {
                    lexer_abort(self, "Illegal character in string.");
                }

                lexer_nextChar(self);

                int stringLen = self->curPos - startPos + 1;
                char* tokenText = (char*)malloc(stringLen);
                strncpy(tokenText, self->source + startPos, stringLen);
                tokenText[stringLen - 1] = '\0';

                token_init(&token, tokenText, STRING);
            }
        }
    }
    else if (self->curChar == '=')
    {
        if (lexer_peek(self) == '=')
        {
            char* chars = malloc(3);
            chars[0] = self->curChar;
            lexer_nextChar(self);
            chars[1] = self->curChar;
            chars[2] = '\0';
            token_init(&token, chars, EQEQ);
        }
        else
        {
            token_init(&token, curTokenChar, EQ);
        }
    }
    else if (self->curChar == '>')
    {
        if (lexer_peek(self) == '=')
        {
            char* chars = malloc(3);
            chars[0] = self->curChar;
            lexer_nextChar(self);
            chars[1] = self->curChar;
            chars[2] = '\0';
            token_init(&token, chars, GTEQ);
        }
        else
        {
            token_init(&token, curTokenChar, GT);
        }
    }
    else if (self->curChar == '<')
    {
        if (lexer_peek(self) == '=')
        {
            char* chars = malloc(3);
            chars[0] = self->curChar;
            lexer_nextChar(self);
            chars[1] = self->curChar;
            chars[2] = '\0';
            token_init(&token, chars, LTEQ);
        }
        else
        {
            token_init(&token, curTokenChar, LT);
        }
    }
    else if (self->curChar == '!')
    {
        if (lexer_peek(self) == '=')
        {
            char* chars = malloc(3);
            chars[0] = self->curChar;
            lexer_nextChar(self);
            chars[1] = self->curChar;
            chars[2] = '\0';
            token_init(&token, chars, NOTEQ);
        }
        else
        {
            char* str = "Expected !=, got !";
            size_t len = strlen(str);
            char* message = (char*)malloc(len + 2);

            strcpy(message, str);
            message[len] = lexer_peek(self);
            message[len + 1] = '\0';
            lexer_abort(self, message);
        }
    }
    else
    {
        // Unknown Token!
        char* str = "Unknown token: ";
        size_t len = strlen(str);
        char* message = (char*)malloc(len + 2);

        if (message == NULL)
        {
            lexer_abort(self, "Failed to allocate memory for message string.");
        }

        strcpy(message, str);
        message[len] = self->curChar;
        message[len + 1] = '\0';
        lexer_abort(self, message);
    }

    return token;
}