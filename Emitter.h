#ifndef EMITTER_H
#define EMITTER_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "Token.h"
#include <ctype.h>

typedef struct Emitter {
    const char* fullPath;
    char* code;
    char* header;
} Emitter;

void emitter_init(Emitter* self, const char* path, size_t fileSize);
void emitter_emit(Emitter* self, char* code);
void emitter_header(Emitter* self, char* code);
void emitter_writeFile(Emitter* self);

#endif