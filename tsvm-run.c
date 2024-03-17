#include <string.h>

#include "opcode.h"
#include "util.h"

// INTERPRETATION

#define MAX_SIZE (1 << 16)

typedef struct {
    uint16_t i;
    uint16_t s;
    uint8_t m[MAX_SIZE];
} VM;

#define POP(n)                                                              \
    (vm->s += (n), (n) == 1   ? *(uint8_t*)&vm->m[(uint16_t)(vm->s - (n))]  \
                   : (n) == 2 ? *(uint16_t*)&vm->m[(uint16_t)(vm->s - (n))] \
                   : (n) == 4 ? *(uint32_t*)&vm->m[(uint16_t)(vm->s - (n))] \
                   : (n) == 8 ? *(uint64_t*)&vm->m[(uint16_t)(vm->s - (n))] \
                              : (assert(false), 0))
#define POPS(n)                                                            \
    (vm->s += (n), (n) == 1   ? *(int8_t*)&vm->m[(uint16_t)(vm->s - (n))]  \
                   : (n) == 2 ? *(int16_t*)&vm->m[(uint16_t)(vm->s - (n))] \
                   : (n) == 4 ? *(int32_t*)&vm->m[(uint16_t)(vm->s - (n))] \
                   : (n) == 8 ? *(int64_t*)&vm->m[(uint16_t)(vm->s - (n))] \
                              : (assert(false), 0))
#define PUSH(n, v)                                              \
    (vm->s -= (n), (n) == 1   ? *(uint8_t*)&vm->m[vm->s] = (v)  \
                   : (n) == 2 ? *(uint16_t*)&vm->m[vm->s] = (v) \
                   : (n) == 4 ? *(uint32_t*)&vm->m[vm->s] = (v) \
                   : (n) == 8 ? *(uint64_t*)&vm->m[vm->s] = (v) \
                              : (assert(false), 0))

bool interpret(VM* vm) {
    vm->i = 0x100;
    vm->s = 0;  // will wrap around on next operation

    while (true) {
        uint8_t instruction = vm->m[vm->i++];
        OpcodeKind oc = instruction & 0x3f;
        OperandSize ss = (instruction >> 6) & 0x3;
        switch (oc) {
            case O_PNK:
                fprintf(stderr, "PNK!: %04X\n", vm->i - 1);
                return false;
                break;
            case O_HLT:
                return true;
                break;
            case O_LIT:
                vm->s -= 1 << ss;
                memcpy(&vm->m[vm->s], &vm->m[vm->i], 1 << ss);
                vm->i += 1 << ss;
                break;
            case O_DUP:
                vm->s -= 1 << ss;
                memcpy(&vm->m[vm->s], &vm->m[vm->s + (1 << ss)], 1 << ss);
                break;
            case O_DRP:
                POP(1 << ss);
                break;
            case O_SWP:
                UNIMPLEMENTED("SWP");
                break;
            case O_OVR:
                UNIMPLEMENTED("OVR");
                break;
            case O_ADD: {
                uint64_t b = POP(1 << ss);
                uint64_t a = POP(1 << ss);
                PUSH(1 << ss, a + b);
            } break;
            case O_SUB: {
                uint64_t b = POP(1 << ss);
                uint64_t a = POP(1 << ss);
                PUSH(1 << ss, a - b);
            } break;
            case O_JMP:
                vm->i = POP(2);
                break;
            case O_JLZ: {
                uint16_t addr = POP(2);
                uint64_t a = POP(1 << ss);
                if (a & 1 << ((8 << ss) - 1)) vm->i = addr;
            } break;
            case O_MUL:
                UNIMPLEMENTED("MUL");
                break;
            case O_SHR:
                UNIMPLEMENTED("SHR");
                break;
            case O_SHL:
                UNIMPLEMENTED("SHL");
                break;
            case O_XOR:
                UNIMPLEMENTED("XOR");
                break;
            case O_AND:
                UNIMPLEMENTED("AND");
                break;
            case O_ORR:
                UNIMPLEMENTED("ORR");
                break;
            case O_STM: {
                uint16_t addr = POP(2);
                memcpy(&vm->m[addr], &vm->m[vm->s], 1 << ss);
                vm->s += 1 << ss;
                if (addr < 0x100) {
                    switch (addr) {
                        // Console
                        case 0x0012:
                            putc(vm->m[addr], stdout);
                            break;
                    }
                }
            } break;
            case O_LDM: {
                uint16_t addr = POP(2);
                PUSH(1 << ss, *(uint64_t*)&vm->m[addr]);
            } break;

            case OPCODE_COUNT:
                break;
        }
    }

    return true;
}

// MAIN

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Expected:\n %s <INPUT>\n", argv[0]);
        return 1;
    }

    VM vm = {0};

    size_t file_size;
    char* file_buffer = slurp_file(argv[1], &file_size);
    if (!file_buffer) {
        return 1;
    }
    memcpy(&vm.m, file_buffer, file_size);
    free(file_buffer);

    if (!interpret(&vm)) {
        fprintf(stderr, "Interpretation ended unsuccessfully\n");
    }
    return vm.m[0];
}
