#ifndef OPCODE_H_
#define OPCODE_H_

#define OPCODES_ENUMERATE \
    OPCODE(PNK)           \
    OPCODE(HLT)           \
    OPCODE(LIT)           \
    OPCODE(DUP)           \
    OPCODE(DRP)           \
    OPCODE(SWP)           \
    OPCODE(OVR)           \
    OPCODE(ADD)           \
    OPCODE(SUB)           \
    OPCODE(JMP)           \
    OPCODE(JLZ)           \
    OPCODE(MUL)           \
    OPCODE(SHR)           \
    OPCODE(SHL)           \
    OPCODE(XOR)           \
    OPCODE(AND)           \
    OPCODE(ORR)           \
    OPCODE(STM)           \
    OPCODE(LDM)

typedef enum {
#define OPCODE(opcode) O_##opcode,
    OPCODES_ENUMERATE
#undef OPCODE
        OPCODE_COUNT
} OpcodeKind;

typedef enum {
    S1 = 0,
    S2 = 1,
    S4 = 2,
    S8 = 3,
} OperandSize;

typedef struct {
    const char* string;
} Opcode;

Opcode opcode_defs[OPCODE_COUNT] = {
#define OPCODE(opcode) {.string = #opcode},
    OPCODES_ENUMERATE
#undef OPCODE
};

#endif
