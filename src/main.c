#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "../include/decoder.h"

int main() {
    Decoder decoder = decoder_init("../asm/many_register_mov");
    char** result = decoder_decode(&decoder);
    if (result == NULL) {
        perror("ERR: Couldn't decode!");
        return EXIT_FAILURE;
    }

    printf("Result:\n");
    for (size_t i = 0; result[i][0]; i++) {
        printf("%s\n", result[i]);
    }

    for (size_t i = 0; result[i]; i++) {
        free(result[i]);
    }

    free(result);
    decoder_deinit(&decoder);

    return EXIT_SUCCESS;
}
