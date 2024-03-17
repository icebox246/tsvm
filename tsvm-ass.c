#include "opcode.h"
#include "util.h"

#define MAX_SIZE 65536

// COMPILATION

ssize_t compile(char* input_buffer, size_t input_buffer_size,
                uint8_t* output_buffer) {
    size_t out_cursor = 0;
    size_t out_size = 0;
    size_t cursor = 0;
    int line = 1;
    int col = 1;
    char* word;
    int last_lit = -1;
    typedef struct {
        char* string;
        struct {
            uint16_t* its;
            size_t cnt;
            size_t cap;
        } uses;
        uint16_t location;
    } Label;
    struct {
        Label* its;
        size_t cnt;
        size_t cap;
    } labels = {0};
    while (cursor < input_buffer_size) {
        char c = input_buffer[cursor];
        bool comment_mode = false;
        while ((is_space(c) || comment_mode ||
                sneq(&input_buffer[cursor], "//", 2)) &&
               cursor < input_buffer_size) {
            if (cursor < input_buffer_size - 1 &&
                sneq(&input_buffer[cursor], "//", 2))
                comment_mode = true;
            cursor++;
            if (c == '\n')
                col = 1, line++, comment_mode = false;
            else
                col++;
            c = input_buffer[cursor];
        }
        if (cursor >= input_buffer_size) break;

        // inline string
        if (input_buffer[cursor] == '"') {
            col++, cursor++;  // chomp "
            c = input_buffer[cursor];
            while (c != '"' && cursor < input_buffer_size) {
                output_buffer[out_cursor++] = c;
                if (c == '\n')
                    col = 1, line++;
                else
                    col++;
                c = input_buffer[++cursor];
            }
            col++, cursor++;  // chomp "
            continue;
        }

        word = &input_buffer[cursor];
        do {
            cursor++;
            c = input_buffer[cursor];
        } while (!is_space(c));
        char space_buf = input_buffer[cursor];
        input_buffer[cursor] = 0;

        size_t wordlen = &input_buffer[cursor] - word;
        bool consumed = false;

        // opcode
        if (wordlen >= 3 && wordlen <= 4) {
            for (int op = 0; op < OPCODE_COUNT; op++) {
                if (sneq(word, opcode_defs[op].string, 3)) {
                    uint8_t opcode = op;
                    uint8_t size = 0;
                    if (wordlen == 4) {
                        switch (word[3]) {
                            case '1':
                                // do nothing
                                break;
                            case '2':
                                size = 1;
                                break;
                            case '4':
                                size = 2;
                                break;
                            case '8':
                                size = 3;
                                break;
                            default:
                                fprintf(
                                    stderr,
                                    "%d:%d: Unsupported operand size '%c'\n",
                                    line, col, word[3]);
                                return -1;
                        }
                    }
                    if (opcode == O_LIT) {
                        last_lit = size;
                    }
                    output_buffer[out_cursor++] = (size << 6) | opcode;
                    consumed = true;
                    break;
                }
            }
        }

        // offset
        if (!consumed && word[0] == ']') {
            bool valid;
            size_t value = hnum(word + 1, wordlen - 1, &valid);
            if (!valid || value > MAX_SIZE) {
                fprintf(
                    stderr,
                    "%d:%d: Invalid offset in absolute memory offset '%s'\n",
                    line, col, word + 1);
                return -1;
            }
            out_cursor = value;
            consumed = true;
        }

        // labels
        if (!consumed && wordlen > 1 && (word[0] == ':' || word[0] == '@')) {
            bool is_use = word[0] == '@';
            bool exists = false;
            size_t i;
            for (i = 0; i < labels.cnt; i++) {
                if (sneq(labels.its[i].string, word + 1, wordlen - 1)) {
                    exists = true;
                    break;
                }
            }
            if (!exists)
                APPEND(labels, (Label){.string = sndup(word + 1, wordlen - 1)});
            if (is_use) {
                output_buffer[out_cursor++] = (1 << 6) | O_LIT;
                APPEND(labels.its[i].uses, out_cursor);
                out_cursor += 2;  // to be backpathed
            } else
                labels.its[i].location = out_cursor;
            consumed = true;
        }

        // number literal
        if (!consumed && (is_num(word[0]) ||
                          (wordlen > 1 && word[0] == '-' && is_num(word[1])) ||
                          word[0] == '\'')) {
            bool valid;
            bool negative = false;
            bool inline_num = false;
            char* num = word;
            size_t numlen = wordlen;
            for (size_t i = 1; i < numlen; i++) {
                if (num[i] == '#') {
                    switch (word[i + 1]) {
                        case '1':
                            last_lit = 0;
                            break;
                        case '2':
                            last_lit = 1;
                            break;
                        case '4':
                            last_lit = 2;
                            break;
                        case '8':
                            last_lit = 2;
                            break;
                        default:
                            fprintf(stderr,
                                    "%d:%d: Unsupported operand size '%c'\n",
                                    line, col, num[i + 1]);
                            return -1;
                    }
                    numlen = i;
                }
            }
            if (num[0] == '\'') {
                inline_num = true;
                numlen--;
                num++;
            }
            if (num[0] == '-') {
                negative = true;
                numlen--;
                num++;
            }
            size_t num_size = 0;
            if (last_lit != -1) num_size = last_lit;
            if (!inline_num) {
                output_buffer[out_cursor++] = (num_size << 6) | O_LIT;
            }
            size_t value = hnum(num, numlen, &valid);
            if (negative) value = ~value + 1;
            if (valid) {
                switch (num_size) {
                    case 0:
                        output_buffer[out_cursor++] = value;
                        break;
                    case 1:
                        *(uint16_t*)&output_buffer[out_cursor] = value;
                        out_cursor += 2;
                        break;
                    case 2:
                        *(uint32_t*)&output_buffer[out_cursor] = value;
                        out_cursor += 4;
                        break;
                    case 3:
                        *(uint64_t*)&output_buffer[out_cursor] = value;
                        out_cursor += 8;
                        break;
                }
                last_lit = -1;
                consumed = true;
            }
        }

        if (!consumed) {
            fprintf(stderr, "%d:%d: Unknown word '%s'\n", line, col, word);
        }
        input_buffer[cursor] = space_buf;
        col += wordlen;
        if (out_cursor > out_size) out_size = out_cursor;
    }

    // backpatch label locations
    for (size_t i = 0; i < labels.cnt; i++) {
        for (size_t j = 0; j < labels.its[i].uses.cnt; j++) {
            *(uint16_t*)&output_buffer[labels.its[i].uses.its[j]] =
                labels.its[i].location;
        }
        free(labels.its[i].uses.its);
        free(labels.its[i].string);
    }
    free(labels.its);

    return out_size;
}

// MAIN

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Expected:\n %s <INPUT> <OUTPUT>\n", argv[0]);
        return 1;
    }

    uint8_t output_buffer[MAX_SIZE];
    ssize_t output_size = 0;

    size_t file_size;
    char* file_buffer = slurp_file(argv[1], &file_size);
    if (!file_buffer) {
        return 1;
    }

    if ((output_size = compile(file_buffer, file_size, output_buffer)) > 0) {
        /* for (size_t i = 0; i < output_size; i++) { */
        /*     if (i % 16 == 0) fprintf(stderr, "\n%0004X: ", (int)i); */
        /*     fprintf(stderr, "%02X ", output_buffer[i]); */
        /* } */
        /* fprintf(stderr, "\n"); */
        write_file(argv[2], (char*)output_buffer, output_size);
    } else {
        fprintf(stderr, "Failed to compile!\n");
    }

    free(file_buffer);
    return 0;
}
