#include "Emitter.h"

void emitter_init(Emitter* self, const char* path, const size_t fileSize)
{
    self->fullPath = path;
    self->header = (char*)malloc(fileSize + 1);
    self->code = (char*)malloc(fileSize + 1);

    self->header[0] = '\0';
    self->code[0] = '\0';
}

void emitter_emit(Emitter* self, char* code)
{
    size_t len = strlen(self->code) + strlen(code);
    strcat(self->code, code);
    self->code[len] = '\0';
}

void emitter_header(Emitter* self, char* code)
{
    size_t len = strlen(self->header) + strlen(code);
    strcat(self->header, code);
    self->header[len] = '\0';
}

void emitter_writeFile(Emitter* self)
{
    FILE* fptr;

    fptr = fopen(self->fullPath, "w");

    fprintf(fptr, self->header);
    fprintf(fptr, self->code);

    fclose(fptr);

    printf("Compile complete.");
}