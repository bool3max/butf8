# butf8

An extremely tiny pure-C library for decoding UTF-8 text that I wrote as an experiment.

It's almost entirely free of any dependencies (including the C stdlib) - it makes one call to `malloc()` but that can very easily be gotten rid of.

## Usage

It's composed of one tiny source file and one header, just clone this repo and link it with whatever you're compiling, for example: 

```
cc -O3 your_prog.c butf8.c -o your_prog
```

```c
#include <string.h>
#include "butf8.h"

const char *unicode_string = "ǉǌćš";

Utf8ParserState *state = utf8_decoder_init_state(unicode_string, strlen(unicode_string));

int codepoint;

while ((codepoint = utf8_decoder_nextchar(state)) > 0) {
    fprintf(stdout, "Codepoint: %.4X\n", codepoint);
}

utf8_decoder_purge_state(state);

/* Get the length of an UTF-8 string: */

fprintf(stdout, "length: %ld\n", utf8_strlen(unicode_string, strlen(unicode_string)));
```

# TODO

* Encoding
* Indexing and modification
