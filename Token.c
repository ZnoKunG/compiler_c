#include "Token.h"

void token_init(Token* self, char* text, enum TokenType type)
{
    self->text = text;
    self->type = type;
}

enum TokenType token_getKeyword(char* text)
{
    if (!strcmp(text, "LABEL"))
    {
        return LABEL;
    }
    else if (!strcmp(text, "GOTO"))
    {
        return GOTO;
    }
    else if (!strcmp(text, "PRINT"))
    {
        return PRINT;
    }
    else if (!strcmp(text, "INPUT"))
    {
        return INPUT;
    }
    else if (!strcmp(text, "LET"))
    {
        return LET;
    }
    else if (!strcmp(text, "IF"))
    {
        return IF;
    }
    else if (!strcmp(text, "THEN"))
    {
        return THEN;
    }
    else if (!strcmp(text, "ENDIF"))
    {
        return ENDIF;
    }
    else if (!strcmp(text, "WHILE"))
    {
        return WHILE;
    }
    else if (!strcmp(text, "REPEAT"))
    {
        return REPEAT;
    }
    else if (!strcmp(text, "ENDWHILE"))
    {
        return ENDWHILE;
    }
    else 
    {
        return IDENT;
    }
}