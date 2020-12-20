#pragma once

#include <stdint.h>
#include <stdbool.h>

/* opaque struct type */
typedef struct _Utf8ParserState Utf8ParserState;

int32_t utf8_decoder_nextchar(Utf8ParserState *save); 

ssize_t utf8_strlen(const char *string, size_t bytelen); 

Utf8ParserState *utf8_decoder_init_state(const char * string, size_t bytelen);
void            utf8_decoder_purge_state(Utf8ParserState *state);
