#include "../include/decoder.h"

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void decoder_deinit(Decoder *self) {
    free(self->instructions);
}

/// read_file will allocate memmory enough
/// to read the entire file.
/// The caller is responsible to free the memmory.
char* read_file(char const *const path) {
    FILE *const fd = fopen(path, "r");
    if (fd == NULL) {
        perror("Error opening file!");
        return NULL;
    }

    fseek(fd, 0, SEEK_END);
    int32_t const filesize = ftell(fd);
    if (filesize == -1) {
        perror("Couldn't determin file size!");
        fclose(fd);
        return NULL;
    }

    errno = 0;
    rewind(fd);
    if (errno) {
        perror("Couldn't determin file size!");
        fclose(fd);
        return NULL;
    }

    char *const encoding = calloc(filesize + 1, sizeof(uint8_t));
    if (encoding == NULL) {
        perror("Not enough memory to read the file!");
        fclose(fd);
        return NULL;
    }

    size_t const read_size = fread(encoding, sizeof(uint8_t), filesize, fd);
    if (read_size != (size_t)filesize) {
        perror("Failed to read complete file");
        free(encoding);
        fclose(fd);
        return NULL;
    }

    fclose(fd);
    return encoding;
}

Decoder decoder_init(char const *const path) {
    return (Decoder) {
        .instructions = read_file(path),
    };
}

char* decode_reg(int const reg, bool const wide) {
    switch (reg) {
        case 0x0:
            if (wide) return "ax";
            return "al";

        case 0x1:
            if (wide) return "cx";
            return "cl";

        case 0x2:
            if (wide) return "dx";
            return "dl";

        case 0x3:
            if (wide) return "bx";
            return "bl";

        case 0x4:
            if (wide) return "sp";
            return "ah";

        case 0x5:
            if (wide) return "bp";
            return "ch";

        case 0x6:
            if (wide) return "si";
            return "dh";

        case 0x7:
            if (wide) return "di";
            return "bh";

        default:
            perror("Invalid register value! Corrupted data!");
    }
    assert(false);
    return NULL;
}

// TODO:
char* decode_effective_addr(uint8_t const reg, bool const wide) {
    if (wide || reg > 1) {
        return NULL;
    }
    return NULL;
}

typedef enum {
    REGMEM_TO_FROM_REG = 0x88,
    IMMEDIAT_TO_REG = 0xB0,
} DECODER_OPCODES;

char** decoder_decode(Decoder const *const self) {
    size_t lines = 0;
    char** result = calloc(1024, sizeof(char *));
    if (result == NULL) {
        perror("Not enough memmory for decoded string\n");
        return NULL;
    }

    for (size_t i = 0; i < 1024 - 1; i++) {
        result[i] = calloc(1024, sizeof(char));
        if (result[i] == NULL) {
            perror("Not enough memmory for decoded string\n");
            return NULL;
        }
    }

    strncpy(result[lines], "bits 16", 9);
    lines++;

    for (size_t i = 0; self->instructions[i]; i++) {
        uint8_t const opcode = self->instructions[i];
        if ((opcode & IMMEDIAT_TO_REG) == IMMEDIAT_TO_REG) {
            if (self->instructions[i + 1] == '\0') {
                perror("Missing load immediat data\n");
                return NULL;
            }

            i += 1;

            uint8_t const data_lo = self->instructions[i];
            bool const wide = (opcode & 0x08) > 0;

            if (wide) {
                if (self->instructions[i + 1] == '\0') {
                    perror("Missing load immediat wide data\n");
                    return NULL;
                }
                i += 1;
            }

            uint16_t data_hi = 0;
            if (wide) data_hi = self->instructions[i];

            char const *const reg = decode_reg(opcode & 0x7, wide);

            uint16_t immediat = data_lo + (data_hi << 8);

            char* buff = calloc(1024, sizeof(char));
            assert(buff != NULL);
            char fmt[] = "mov %s, %d";
            size_t fmt_size = sprintf(buff, fmt, reg, immediat);

            strncpy(result[lines], buff, fmt_size);
            free(buff);

            lines++;
        } else if ((opcode & REGMEM_TO_FROM_REG) == REGMEM_TO_FROM_REG) {
            if (self->instructions[i + 1] == '\0') {
                perror("Missing reg/mem to/from reg data\n");
                return NULL;
            }

            i += 1;
            uint8_t const operand = self->instructions[i];

            // 1 for word, 0 for byte
            bool const wide = (opcode & 0x01) > 0;

            uint8_t const reg = (operand & 0x38) >> 3;
            char const *const reg_field1 = decode_reg(reg, wide);

            uint8_t const reg_or_mem = operand & 0x07;

            // 0 means that REG is the source
            // 1 means that REG is the destination
            bool const direction = (opcode & 0x02) > 0;
            char const * reg_field2 = decode_reg(reg_or_mem, wide);
            if (direction) {
                reg_field2 = decode_effective_addr(reg_or_mem, wide);
            }

            // const mod = (operand & 0b11000000) >> 6;

            char *const buff = calloc(1024, sizeof(char));
            assert(buff != NULL);

            char const fmt[] = "mov %s, %s";
            size_t const fmt_size = sprintf(buff, fmt, reg_field2, reg_field1);

            strncpy(result[lines], buff, fmt_size);
            free(buff);

            lines++;
        } else {
            perror("Unsupported opcode\n");
        }
    }
    return result;
}
