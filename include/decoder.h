#ifndef DECODER_H

#define DECODER_H

#include <stdint.h>

typedef struct {
    char *const instructions;
} Decoder;

/// make sure to call `decoder_deinit` to free
/// the memory.
Decoder decoder_init(const char *const path);

/// frees the instrucions read from the file
void decoder_deinit(Decoder *self);

/// it returns the decoded program or NULL in case of a fail
/// The user owns the data and he is the one to free it.
char** decoder_decode(const Decoder *const self);

#endif
