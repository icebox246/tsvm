#include <stdio.h>

#include "opcode.h"

int main() {
    FILE* of = fopen("vim/syntax/tsvm-assembly.vim", "w");

    // String
    fprintf(of, "syntax region TSAString start=/\\v\"/ end=/\\v\"/\n");
    fprintf(of, "highligh link TSAString String\n");

    // Number
    fprintf(of,
            "syntax match TSANumber /-\\?[0-9a-fA-F]\\+\\(#[1248]\\)\\?/\n");
    fprintf(of, "highligh link TSANumber Number\n");

    // Immediate
    fprintf(of,
            "syntax match TSAImm /'-\\?[0-9a-fA-F]\\+\\(#[1248]\\)\\?/\n");
    fprintf(of, "highligh link TSAImm Character\n");

    // Offsets
    fprintf(of, "syntax match TSAOffset /\\][0-9a-fA-F]\\+/\n");
    fprintf(of, "highligh link TSAOffset PreProc\n");

    // Mnemonics
    fprintf(of, "syn keyword TSAMnemonic ");
    for (size_t i = 0; i < OPCODE_COUNT; i++) {
        fprintf(of, "%s ", opcode_defs[i].string);
        for (size_t s = 0; s < 4; s++) {
            fprintf(of, "%s%d ", opcode_defs[i].string, 1 << s);
        }
    }
    fprintf(of, "\n");
    fprintf(of, "highligh link TSAMnemonic Keyword\n");

    // Label
    fprintf(of, "syntax match TSALabel /[@:]\\S\\+/\n");
    fprintf(of, "highligh link TSALabel Text\n");

    // Comment
    fprintf(of, "syntax match TSAComment \"\\v//.*$\"\n");
    fprintf(of, "highligh link TSAComment Comment\n");

    fclose(of);
}
