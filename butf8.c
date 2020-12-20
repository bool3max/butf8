#include <stdint.h>
#include <stdlib.h>

#include "headers/butf8.h"

#define is_1byte(b) ((b & 0x80) == 0) // ascii-compatible single-byte codepoint
#define is_2byte(b) (((b & 0xC0) == 0xC0) && ((b & 0x20) == 0))
#define is_3byte(b) (((b & 0xE0) == 0xE0) && ((b & 0x10) == 0))
#define is_4byte(b) (((b & 0xF0) == 0xF0) && ((b & 0x08) == 0))

#define extract_2byte(b)  ((b) & 0x1F) // extract bits from the form:           110xxxxx
#define extract_3byte(b)  ((b) & 0xF)  // extract bits from the form:           1110xxxx
#define extract_4byte(b)  ((b) & 0x7)  // extract bits from the form:           11110xxx
#define extract_beyond(b) ((b) & 0x3F) // extract bits of the byte in the form: 10xxxxxx

// don't expose this struct to the API
struct _Utf8ParserState {
    const char *string;
    size_t bytelen;
    int32_t byte_pos;
};

int32_t utf8_decoder_nextchar(Utf8ParserState *state) {
    /* Return values: 
     * 0 : end of string (i.e. don't call this function again - we parsed the whole string)
     * -1: error parsing
     * > 0: actual Unicode codepoint
     */

    /* The function doesn't take into account the null-terminating byte of the string, but instead goes off of the provided string length */

    if(state->byte_pos > (state->bytelen - 1))
        // we've reached end of string - this is a sign for the API consumer not to call the function again - and for them to call utf8_decoder_purge_state if they wish to free the resources used by the state
        return 0;

    const uint8_t current_byte = (state->string)[state->byte_pos];

    if(current_byte == 0x0)
        // there's a null-terminating-byte in the middle of the string for some reason, abort
        return 0;


    if(is_1byte(current_byte)) {
        // since 'current_byte' is an unsigned type we could also check 'current_byte <= 0x7F' but this is more 'portable'
        // we're within the ASCII-compatibility range, so the actual Unicode codepoint is just the current byte
        state->byte_pos++;
        return current_byte;
    }

    if(is_2byte(current_byte)) {
        int32_t ret = (extract_2byte(current_byte) << 6) | extract_beyond((state->string)[state->byte_pos + 1]);

        state->byte_pos += 2;
        return ret;
    }
    
    if(is_3byte(current_byte)) {
        int32_t ret = ( (  (extract_3byte(current_byte) << 6) | extract_beyond((state->string)[state->byte_pos + 1]) ) << 6 ) | extract_beyond((state->string)[state->byte_pos + 2]);
        state->byte_pos += 3;
        return ret;
    }

    if(is_4byte(current_byte)) {
        int32_t ret = (((((extract_3byte(current_byte) << 6) | extract_beyond((state->string)[state->byte_pos + 1]) ) << 6 ) | extract_beyond((state->string)[state->byte_pos + 2]) ) << 6) | extract_beyond((state->string)[state->byte_pos + 3]);

        state->byte_pos += 4;
        return ret;
    }
}

Utf8ParserState *utf8_decoder_init_state(const char *string, size_t bytelen) {
    struct _Utf8ParserState *state = malloc(sizeof(struct _Utf8ParserState));
    if(state == NULL)
        return state;

    *state = (struct _Utf8ParserState) {
        .string = string,
        .byte_pos = 0,
        .bytelen = bytelen
    };

    return state;
}

void utf8_decoder_purge_state(Utf8ParserState *state) {
    free(state);
}

ssize_t utf8_strlen(const char *string, size_t bytelen) {
    ssize_t len = 0;
    char *p = string;

    while(p < (string + bytelen)) {
        if(is_1byte(*p))
            p++;
         else if(is_2byte(*p)) 
            p += 2;
         else if(is_3byte(*p)) 
            p += 3;
        else if(is_4byte(*p)) 
            p += 4;
        else {
            /* Invalid UTF-8 byte */
            return -1;
        }

        len++;
    }

    return len;
}
