#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void xor_encrypt(char *input, size_t sDataSize, const char *key) {
    size_t keyLen = strlen(key);
    for (size_t i = 0; i < sDataSize; i++) {
        input[i] ^= key[i % keyLen];
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    const char key[] = "HB/783-/48@Nvfebv";
    FILE *inputFile = fopen(argv[1], "rb");
    if (!inputFile) {
        perror("Error opening input file");
        return 1;
    }

    FILE *outputFile = fopen("payload.xor", "wb");
    if (!outputFile) {
        perror("Error opening output file");
        fclose(inputFile);
        return 1;
    }

    fseek(inputFile, 0, SEEK_END);
    size_t fileSize = ftell(inputFile);
    rewind(inputFile);

    char *buffer = (char *)malloc(fileSize);
    if (!buffer) {
        perror("Memory allocation error");
        fclose(inputFile);
        fclose(outputFile);
        return 1;
    }

    fread(buffer, 1, fileSize, inputFile);
    xor_encrypt(buffer, fileSize, key);
    fwrite(buffer, 1, fileSize, outputFile);

    free(buffer);
    fclose(inputFile);
    fclose(outputFile);

    printf("Encryption completed. Output saved to payload.xor\n");
    return 0;
}

