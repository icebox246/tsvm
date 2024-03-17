#ifndef UTIL_H_
#define UTIL_H_

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef uint8_t bool;
#define true 1
#define false 0

#define UNIMPLEMENTED(s) assert(false && s " is not implemented yet")

#define APPEND(arr, it)                                                       \
    do {                                                                      \
        if ((arr).cnt == (arr).cap) {                                         \
            if ((arr).cap == 0) (arr).cap = 4;                                \
            (arr).cap *= 2;                                                   \
            (arr).its = realloc((arr).its, sizeof((arr).its[0]) * (arr).cap); \
        }                                                                     \
        (arr).its[(arr).cnt++] = (it);                                        \
    } while (0)

bool is_space(char c) { return c == ' ' || c == '\t' || c == '\n' || c == 0; }
bool is_word(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
bool is_num(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') ||
           (c >= 'a' && c <= 'f');
}

bool sneq(const char* a, const char* b, size_t n) {
    while (n--)
        if (*a++ != *b++) return false;
    return true;
}

char* sndup(const char* a, size_t n) {
    char* res = malloc(n + 1);
    res[n] = 0;
    while (n--) res[n] = a[n];
    return res;
}

int hdig(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    fprintf(stderr, "Non hex char '%c'!\n", c);
    return -1;
}

size_t hnum(char* word, size_t wordlen, bool* valid) {
    if (valid) *valid = true;
    size_t value = 0;
    bool negative = false;
    for (size_t i = 0; i < wordlen; i++) {
        int dig = hdig(word[i]);
        if (dig < 0) {
            if (valid) *valid = false;
            return 0;
        }
        value = value * 16 + hdig(word[i]);
    }
    return value;
}

char* slurp_file(char* file_name, size_t* out_file_size) {
    FILE* input_file = fopen(file_name, "rb");
    if (!input_file) {
        perror("Failed to open file for reading");
        return NULL;
    }
    fseek(input_file, 0, SEEK_END);
    size_t file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);
    char* file_buffer = (char*)malloc(file_size + 1);
    fread(file_buffer, 1, file_size, input_file);
    file_buffer[file_size] = 0;
    fclose(input_file);

    if (out_file_size) *out_file_size = file_size;
    return file_buffer;
}

void write_file(char* file_name, char* data, size_t data_size) {
    FILE* output_file = fopen(file_name, "wb");
    if (!output_file) {
        perror("Failed to open file for writing");
        return;
    }
    fwrite(data, 1, data_size, output_file);
    fclose(output_file);
}

#endif
