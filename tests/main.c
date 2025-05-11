#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../include/decoder.h"

/// RUN_TEST requires the user to first declare char* err_msg = NULL
/// before making use of RUN_TEST
#define RUN_TEST(test_name) do { \
    err_msg = test_name(); \
    if (strlen(err_msg) == 0) { \
        printf("TEST: \033[33m'%s'\033[0m ........... \033[32mPASSED\033[0m\n", #test_name); \
    } else { \
        printf("TEST: \033[33m'%s'\033[0m ........... \033[31mFAILED\033[0m\n", #test_name); \
        printf("ERR: %s", err_msg); \
        printf("\n"); \
        free(err_msg); \
    } \
} while(0)

char* test_load_immediat() {
    char *err_msg = calloc(256, sizeof(char));

    Decoder decoder = decoder_init("../asm/load_immediat");
    char** result = decoder_decode(&decoder);
    assert(result != NULL);

    char expected[3][20]  = {
        "bits 16",
        "mov ax, 300",
        "mov bl, 22",
    };

    for (size_t i = 0; i < 3; i++) {
        if (strcmp(expected[i], result[i]) != 0) {
            char* buff = calloc(100, sizeof(char));
            const char fmt[] = "ERROR: i=%ld, expected '%s' got '%s'\n";
            const size_t fmt_size = sprintf(buff, fmt, i, expected[i], result[i]);
            strncpy(err_msg, buff, fmt_size);
            free(buff);
            break;
        }
    }

    for (size_t i = 0; result[i]; i++) {
        free(result[i]);
    }

    free(result);
    decoder_deinit(&decoder);

    return err_msg;
}

char* test_single_register_mov() {
    char *err_msg = calloc(256, sizeof(char));

    Decoder decoder = decoder_init("../asm/single_register_mov");
    char** result = decoder_decode(&decoder);
    assert(result != NULL);

    char expected[2][20]  = {
        "bits 16",
        "mov cx, bx"
    };

    for (size_t i = 0; i < 2; i++) {
        if (strcmp(expected[i], result[i]) != 0) {
            char* buff = calloc(100, sizeof(char));
            const char fmt[] = "ERROR: i=%ld, expected '%s' got '%s'\n";
            const size_t fmt_size = sprintf(buff, fmt, i, expected[i], result[i]);
            strncpy(err_msg, buff, fmt_size);
            free(buff);
            break;
        }
    }

    for (size_t i = 0; result[i]; i++) {
        free(result[i]);
    }

    free(result);
    decoder_deinit(&decoder);

    return err_msg;
}

char* test_many_register_mov() {
    char *err_msg = calloc(256, sizeof(char));

    Decoder decoder = decoder_init("../asm/many_register_mov");
    char** result = decoder_decode(&decoder);
    assert(result != NULL);

    char expected[12][20]  = {
        "bits 16",
        "mov cx, bx",
        "mov ch, ah",
        "mov dx, bx",
        "mov si, bx",
        "mov bx, di",
        "mov al, cl",
        "mov ch, ch",
        "mov bx, ax",
        "mov bx, si",
        "mov sp, di",
        "mov bp, ax",
    };

    for (size_t i = 0; i < 2; i++) {
        if (strcmp(expected[i], result[i]) != 0) {
            char* buff = calloc(100, sizeof(char));
            const char fmt[] = "ERROR: i=%ld, expected '%s' got '%s'\n";
            const size_t fmt_size = sprintf(buff, fmt, i, expected[i], result[i]);
            strncpy(err_msg, buff, fmt_size);
            free(buff);
            break;
        }
    }

    for (size_t i = 0; result[i]; i++) {
        free(result[i]);
    }

    free(result);
    decoder_deinit(&decoder);

    return err_msg;
}

int main() {
    // RUN_TEST requires err_msg to work
    char* err_msg = NULL;
    RUN_TEST(test_load_immediat);
    RUN_TEST(test_single_register_mov);
    RUN_TEST(test_many_register_mov);

    return EXIT_SUCCESS;
}
