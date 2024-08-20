#include <stdio.h>
#include "Lexer.c"
#include "Parser.c"
#include "Emitter.c"

void readFile(char* result, const char* fileName);

int main()
{
    const size_t FILESIZE = 300;
    char* source = (char*)malloc(FILESIZE);
    const char inputFileName[] = "program.txt";
    const char outputFileName[] = "out.c";
    readFile(source, inputFileName);
    
    // Initialize Lexer and Parser
    Lexer lex;
    lexer_init(&lex, source);
    Parser parser;
    Emitter emitter;
    emitter_init(&emitter, outputFileName, FILESIZE);
    parser_init(&parser, &lex, &emitter);

    parser_start(&parser);
    emitter_writeFile(&emitter);

    // Token token;

    // while (lexer_peek(&lex) != '\0')
    // {
    //     token = lexer_getToken(&lex);
    //     printf("Character: %s, Type: %d\n", token.text, token.type);
    //     lexer_nextChar(&lex);
    // }

    return 0;
}

void readFile(char* result, const char* fileName)
{
    FILE* fptr;
    char line[50];

    fptr = fopen(fileName, "r");

    if(fptr == NULL)
    {
        printf("Not able to open the file.");
        abort();
    }

    printf("Reading file %s\n", fileName);

    fgets(line, 50, fptr);
    strcpy(result, line);
    strcat(result, "\n");
    printf("%s", line);
    
    while (fgets(line, 50, fptr) != NULL)
    {
        strcat(result, line);
        strcat(result, "\n");
        printf("%s", line);
    }

    printf("\n");
    fclose(fptr);
}