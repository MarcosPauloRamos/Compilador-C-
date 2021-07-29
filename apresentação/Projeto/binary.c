#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "assembly.h"
#include <string.h>

#define END_SWITCH 67

const char *Prefixos[] = { "add", "sub", "mult", "div", "and", "or", "nand", "nor", "sle", "slt", "sge", "addi", "subi", "divi", "multi", "andi", "ori",
                             "nori", "slei", "slti", "beq", "bne", "blt", "bgt", "sti", "ldi", "str", "ldr", "halt", "in", "out", "jmp", "jal", "jst",
                             "sleep", "wake", "lstk", "sstk", "mov", "put", "ctso" };

const char *opcodeBins[] =   {"000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", "000000", 
                              "000001", "000010", "000011", "000100", "000101", "000110", "000111", "001000", "001001", "001010", "001011",
                              "001100", "001101", "001110", "001111", "010000", "010001", "010010", "010011", "010100", "010101", "010110",
                              "010111", "010110", "011010", "011011", "011100", "011101"};

const char *functBins[] = { "000000", "000001", "000010", "000011", "000100", "000101", "000110", "000111", "001000", "001001", "001010" };

/*int transformacaobinario (int bin, int tamanho, char *nbinario){
    int x[32];
    char aux[50], n[50];
    int i, j, numero = bin;
    for (i = 0; i < 32; i++) x[i] = 0;
    i = 31;
    while (numero > 1){
        x[i] = numero%2;
        numero = numero/2;
        i--;
    }
    x[i] = numero%2;
    n[0] ='\0';
    for (j = 32 - tamanho; j < 32; j++){
        sprintf(aux, "%d", x[j]);
        strcat(n, aux);
    }
    strcpy (nbinario, n);
    return 0;
}*/

void assembly_binary(AssemblyCode codeLine){
    Instruction inst;
    char r1[5], r2[5], r3[5],ri[26];
    if(codeLine->kind == instr){
        inst = codeLine->line.instruction;
        switch(inst.format){
        case formatR:
            /*
            transformacaobinario(inst.reg1, 5, r1);
            transformacaobinario(inst.reg2, 5, r2);
            transformacaobinario(inst.reg3, 5, r3);
            transformacaobinario(0, 5, ri);
            fprintf(listing,"ram[%d] = {6'b%s, 5'b%s, 5'b%s, 5'b%s, 5'b%s, 6'b%s};",codeLine->lineno,
                                                             opcodeBins[inst.opcode],
                                                             r2,
                                                             r3,
                                                             r1,
                                                             ri,
                                                             functBins[inst.opcode]);
            fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            break;
            */
            fprintf(listing,"ram[%d] = {6'b%s, 5'd%d, 5'd%d, 5'd%d, 5'd0, 6'b%s};",codeLine->lineno,
                                                             opcodeBins[inst.opcode],
                                                             inst.reg2,
                                                             inst.reg3,
                                                             inst.reg1,
                                                             functBins[inst.opcode]);
            fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            break;
        case formatJ:
            if(inst.opcode == jst){
              /*
              transformacaobinario(0, 26, ri);
                fprintf(listing,"ram[%d] = {6'b%s, 26'b%s};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           ri);
              */
                fprintf(listing,"ram[%d] = {6'b%s, 26'd0};",codeLine->lineno,
                                                           opcodeBins[inst.opcode]);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else{
              /*
              transformacaobinario(inst.imed, 26, ri);
                fprintf(listing,"ram[%d] = {6'b%s, 26'b%s};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           ri);
              */
                fprintf(listing,"ram[%d] = {6'b%s, 26'd%d};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.imed);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }
            break;
        case formatI:
            if(inst.opcode == sti || inst.opcode == ldi){
              /*
            transformacaobinario(inst.reg1, 5, r1);
            transformacaobinario(0, 5, r2);
            transformacaobinario(inst.imed, 16, ri);
                fprintf(listing,"ram[%d] = {6'b%s, 5'b%s, 5'b%s, 16'b%s};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           r2,
                                                           r1,
                                                           ri);
              */
                fprintf(listing,"ram[%d] = {6'b%s, 5'd0, 5'd%d, 16'd%d};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.reg1,
                                                           inst.imed);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else if(inst.opcode == bgt || inst.opcode == blt){
              /*
            transformacaobinario(inst.reg1, 5, r1);
            transformacaobinario(inst.reg2, 5, r2);
            transformacaobinario(inst.imed, 16, ri);
                fprintf(listing,"ram[%d] = {6'b%s, 5'b%s, 5'b%s, 16'b%s};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           r1,
                                                           r2,
                                                           ri);
              */
                fprintf(listing,"ram[%d] = {6'b%s, 5'd%d, 5'd%d, 16'd%d};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.reg1,
                                                           inst.reg2,
                                                           inst.imed);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else if(inst.opcode == mov || inst.opcode == put){
              /*
            transformacaobinario(inst.reg1, 5, r1);
            transformacaobinario(inst.reg2, 5, r2);
            transformacaobinario(inst.imed, 16, ri);
                fprintf(listing,"ram[%d] = {6'b%s, 5'b%s, 5'b%s, 16'b%s};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           r2,
                                                           r1,
                                                           ri);
              */
                fprintf(listing,"ram[%d] = {6'b%s, 5'd%d, 5'd%d, 16'd%d};",codeLine->lineno,
                                                           opcodeBins[addi],
                                                           inst.reg2,
                                                           inst.reg1,
                                                           inst.imed);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else{
              /*
            transformacaobinario(inst.reg1, 5, r1);
            transformacaobinario(inst.reg2, 5, r2);
            transformacaobinario(inst.imed, 16, ri);
                fprintf(listing,"ram[%d] = {6'b%s, 5'b%s, 5'b%s, 16'b%s};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           r2,
                                                           r1,
                                                           ri);
              */
                fprintf(listing,"ram[%d] = {6'b%s, 5'd%d, 5'd%d, 16'd%d};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.reg2,
                                                           inst.reg1,
                                                           inst.imed);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }
            break;
        case formatSYS:
            if(inst.opcode == halt || inst.opcode == sleep || inst.opcode == wake){
              /*
              transformacaobinario(0, 26, ri);
              fprintf(listing,"ram[%d] = {6'b%s, 26'b%s};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           ri);
              */
                fprintf(listing,"ram[%d] = {6'b%s, 26'd0};",codeLine->lineno,
                                                           opcodeBins[inst.opcode]);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else if(inst.opcode == ctso){
              /*
              transformacaobinario(END_SWITCH, 26, ri);
              fprintf(listing,"ram[%d] = {6'b%s, 26'b%s};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           ri);
              */
                fprintf(listing,"ram[%d] = {6'b%s, 26'd%d};",codeLine->lineno,
                                                           opcodeBins[jal],END_SWITCH);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }else{
              /*
              transformacaobinario(0, 5, r1);
            transformacaobinario(inst.reg2, 5, r2);
            transformacaobinario(inst.reg3, 5, r3);
            transformacaobinario(0, 16, ri);
              fprintf(listing,"ram[%d] = {6'b%s, 5'b%s, 5'b%s, 16'b%s};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           r1,
                                                           r2,
                                                           ri);
              */
                fprintf(listing,"ram[%d] = {6'b%s, 5'd0, 5'd%d, 16'd0};",codeLine->lineno,
                                                           opcodeBins[inst.opcode],
                                                           inst.reg2);
                fprintf(listing,"   // %s\n",Prefixos[inst.opcode]);
            }
            break;
        }
    }else{
        fprintf(listing,"// %s\n",codeLine->line.label);
    }
}

void generateBinary () {
    AssemblyCode aux = getAssembly();
    char *bin;

    if(PrintCode)
        printf( "\nCódigo Binário:\n" );
    while (aux != NULL && listing != NULL) {
        assembly_binary(aux);
        aux = aux->next;
    }
    PrintCode = FALSE;
}
