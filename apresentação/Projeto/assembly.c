#include "globals.h"
#include "symtab.h"
#include "cgen.h"
#include "assembly.h"

#define QUANTUM 20

const char *InstrNames[] = { "add", "sub", "mult", "div", "and", "or", "nand", "nor", "sle", "slt", "sge", "addi", "subi", "divi", "multi", "andi", "ori",
                             "nori", "slei", "slti", "beq", "bne", "blt", "bgt", "sti", "ldi", "str", "ldr", "halt", "in", "out", "jmp", "jal", "jst",
                             "sleep", "wake", "lstk", "sstk", "mov", "put", "ctso" };

const char * regNames[] = { "$zero", "$r1", "$r2", "$r3", "$r4", "$r5", "$r6", "$r7", "$r8", "$r9", "$r10", "$r11", "$r12", "$r13", "$r14", "$r15",
                            "$r16", "$r17", "$r18", "$r19", "$p1", "$p2", "$p3", "$p4", "$p5", "$p6", "$p7", "$p8", "$p9", "$p10", "$ret", "$lp","" };

AssemblyCode codehead = NULL;
FunList funlisthead = NULL;

int line;
int nscopes = 0;
int curparam = 0;
int curarg = 0;
int narg = 0;
int jmpmain = 0;
int switch_SO = 0;

void Funcao (char * id) {
    FunList inserido = (FunList) malloc(sizeof(struct FunListRec));
    inserido->id = (char *) malloc(strlen(id) * sizeof(char));
    strcpy(inserido->id, id);
    inserido->size = 0;
    inserido->next = NULL;
    if (funlisthead == NULL) {
        funlisthead = inserido;
    }
    else {
        FunList func = funlisthead;
        while (func->next != NULL) func = func->next;
        func->next = inserido;
    }
    nscopes ++;
}

void insertVar (char * scope, char * id, int size, VarKind kind) {
    FunList func = funlisthead;
    if(scope == NULL){
      if(kind == 1 )
        scope= func->id;
      else
        scope= func->next->id;
    }
    while (func != NULL && strcmp(func->id, scope) != 0)  func = func->next;
      if (func == NULL) {
        Funcao(scope);
        func = funlisthead;
        while (func != NULL && strcmp(func->id, scope) != 0 ) 
        func = func->next;
    }
    VarList inserido = (VarList) malloc(sizeof(struct VarListRec));
    inserido->id = (char *) malloc(strlen(id) * sizeof(char));
    strcpy(inserido->id, id);
    inserido->size = size;
    inserido->memloc = getMemLoc(id,scope);
    inserido->kind = kind;
    inserido->next = NULL;
    if (func->vars == NULL) {
        func->vars = inserido;
    }
    else {
        VarList v = func->vars;
        while (v->next != NULL) v = v->next;
        v->next = inserido;
    }
    
    func->size = func->size + size;
}

VarKind checkType (QuadList l) {
    QuadList aux = l;
    Quad q = aux->quad;
    aux = aux->next;
    while (aux != NULL && aux->quad.op != opEND) {
        if (aux->quad.op == opVEC && strcmp(aux->quad.addr2.contents.var.name, q.addr1.contents.var.name) == 0) return address;
        aux = aux->next;
    }
    return simple;
}

void insertLabel (char * label) {
    AssemblyCode inserido = (AssemblyCode) malloc(sizeof(struct AssemblyCodeRec));
    inserido->lineno = line;
    inserido->kind = lbl;
    inserido->line.label = (char *) malloc(strlen(label) * sizeof(char));
    strcpy(inserido->line.label, label);
    inserido->next = NULL;
    if (codehead == NULL) {
        codehead = inserido;
    }
    else {
        AssemblyCode aux = codehead;
        while (aux->next != NULL) aux = aux->next;
        aux->next = inserido;
    }
}

void formatogeral (InstrFormat format, InstrKind opcode, Reg reg1, Reg reg2, Reg reg3, int imed, char * label) {
    Instruction instrucao;
    switch_SO++;
    if(opcode == ctso) switch_SO = 0;
    instrucao.format = format;
    instrucao.opcode = opcode;
    instrucao.reg1 = reg1;
    instrucao.reg2 = reg2;
    instrucao.reg3 = reg3;
    instrucao.imed = imed;
    if (label != NULL) {
        instrucao.label = (char *) malloc(strlen(label) * sizeof(char));
        strcpy(instrucao.label, label);
    }
    AssemblyCode inserido = (AssemblyCode) malloc(sizeof(struct AssemblyCodeRec));
    inserido->lineno = line;
    inserido->kind = instr;
    inserido->line.instruction = instrucao;
    inserido->next = NULL;
    if (codehead == NULL) {
        codehead = inserido;
    }
    else {
        AssemblyCode aux = codehead;
        while (aux->next != NULL) aux = aux->next;
        aux->next = inserido;
    }
    line ++;
}

void registrador(InstrKind opcode, Reg rf, Reg r1, Reg r2){
    formatogeral(formatR, opcode, rf, r1,r2, 0, NULL);
}

void imediato(InstrKind opcode, Reg rf, Reg r1, int imed, char *label){
    formatogeral(formatI,opcode,rf,r1,$zero,imed,label);
}

void sistema(InstrKind opcode, Reg rf){
    formatogeral(formatSYS,opcode,$zero,rf,$zero,0,NULL);
}

void salto (InstrKind opcode, int im, char * imlbl) {
    formatogeral(formatJ, opcode, $zero, $zero, $zero, im, imlbl);
}

Reg getParamReg () {
    return (Reg) nregtemp + curparam;
}

Reg getArgReg () {
    return (Reg) nregtemp + curarg;
}

Reg getReg (char * regName) {
    for (int i = 0; i < nregisters; i ++) {
        if (strcmp(regName, regNames[i]) == 0) return (Reg) i;
    }
    return $zero;
}

int getLabelLine (char * label) {
    AssemblyCode aux = codehead;
    while (aux->next != NULL) {
        if (aux->kind == lbl && strcmp(aux->line.label, label) == 0) return aux->lineno;
        aux = aux->next;
    }
    return -1;
}

VarKind getVarKind (char * id, char * scope) {
    FunList funcao = funlisthead;
    while (funcao != NULL && strcmp(funcao->id, scope) != 0) funcao = funcao->next;
    if (funcao == NULL) {
        return simple;
    }
    VarList variavel = funcao->vars;
    while (variavel != NULL) {
        if (strcmp(variavel->id, id) == 0) return variavel->kind;
        variavel = variavel->next;
    }
    return simple;
}

int getFunSize (char * id) {
    FunList funcao = funlisthead;
    while (funcao != NULL && strcmp(funcao->id, id) != 0) funcao = funcao->next;
    if (funcao == NULL) return -1;
    return funcao->size;
}

void initCode (QuadList head) {
    imediato(addi,$lp,$zero,lploc,NULL);
    Funcao("global");
}

void inserirInstrucao (QuadList l) {
    Quad q;
    Address a1, a2, a3;
    int aux;
    VarKind v;

    while (l != NULL) {

        q = l->quad;
        a1 = q.addr1;
        a2 = q.addr2;
        a3 = q.addr3;
        FunList g = funlisthead;
        switch (q.op) {
            
            case opMOV:
                imediato(mov, getReg(a1.contents.var.name), getReg(a2.contents.var.name), a3.contents.val, NULL);
                break;

            case opPUT:
                imediato(put, getReg(a1.contents.var.name), getReg(a2.contents.var.name), a3.contents.val, NULL);
                break;
            
            case opADD:
                registrador(add, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opSUB:
                registrador(sub, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opMULT:
                registrador(mult, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opDIV:
                registrador(divi, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opSLE:
                registrador(sle, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opSGE:
                registrador(sge, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opAND:
                registrador(and, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opOR:
                registrador(or, getReg(a1.contents.var.name), getReg(a2.contents.var.name), getReg(a3.contents.var.name));
                break;
    
            case opBGT:
                imediato(bgt, getReg(a1.contents.var.name), getReg(a2.contents.var.name), -1, a3.contents.var.name);
                break;
    
            case opBLT:
                imediato(blt, getReg(a1.contents.var.name), getReg(a2.contents.var.name), -1, a3.contents.var.name);
                break;
    
            case opBEQ:
                imediato(beq, getReg(a1.contents.var.name), getReg(a2.contents.var.name), -1, a3.contents.var.name);
                break;
    
            case opBNE:
                imediato(bne, getReg(a1.contents.var.name), getReg(a2.contents.var.name), -1, a3.contents.var.name);
                break;
            
            case opATRIB:
                imediato(mov, getReg(a1.contents.var.name), getReg(a2.contents.var.name),0,NULL);
                break;
            
            case opALLOC:
                if (a2.contents.val == 1){
                 insertVar(a3.contents.var.scope, a1.contents.var.name, a2.contents.val, simple);
                }
                else insertVar(a3.contents.var.scope, a1.contents.var.name, a2.contents.val, vector);
                break;
            
            case opADDI:
                imediato(addi, getReg(a1.contents.var.name), getReg(a2.contents.var.name), a3.contents.val, NULL);
                break;
    
            case opLOAD:
                aux = getMemLoc(a2.contents.var.name,a2.contents.var.scope);
                if (aux == -1) { // caso a variável for global
                    aux = getMemLoc(a2.contents.var.name, "global");
                    imediato(ldi, getReg(a1.contents.var.name), none, aux, NULL);
                }else 
                    imediato(ldr, getReg(a1.contents.var.name), $lp, aux, NULL);
                break;

            case opVEC:
                if(getVarKind(a2.contents.var.name,a2.contents.var.scope) == address){
                    imediato(ldr,getReg(a1.contents.var.name),$lp,getMemLoc(a2.contents.var.name,a2.contents.var.scope),NULL);
                    registrador(add,getReg(a3.contents.var.name),getReg(a3.contents.var.name),getReg(a1.contents.var.name));
                    imediato(ldr,getReg(a1.contents.var.name),getReg(a3.contents.var.name),0,NULL);
                }else{
                    aux = getMemLoc(a2.contents.var.name,a2.contents.var.scope);
                    if (aux == -1) { // caso a variável for global
                        aux = getMemLoc(a2.contents.var.name, "global");
                        imediato(ldr,getReg(a1.contents.var.name),getReg(a3.contents.var.name),aux,NULL);    
                    }else{
                        imediato(ldr,getReg(a1.contents.var.name),getReg(a3.contents.var.name),aux,NULL);
                    }
                }break;

            case opSTORE:
                aux = getMemLoc(a2.contents.var.name, a2.contents.var.scope);
                if (aux == -1){ // caso seja uma variavel global
                    aux = getMemLoc(a2.contents.var.name, "global");

                    if(a3.kind == Empty) // caso não seja um vetor global
                        imediato(sti, getReg(a1.contents.var.name), none, aux, NULL);
                    else if(a3.kind == IntConst){
                        aux += a3.contents.val-1;
                        imediato(sti, getReg(a1.contents.var.name), none, aux, NULL);

                    }else imediato(str,getReg(a1.contents.var.name),getReg(a3.contents.var.name),aux,NULL);
                    
                }else if(a3.kind == Empty) // caso não seja um vetor local
                    imediato(str, getReg(a1.contents.var.name), $lp, aux, NULL);
                
                else if(a3.kind == IntConst){ // caso seja um vetor local
                    aux += a3.contents.val-1;
                    imediato(str, getReg(a1.contents.var.name), $lp, aux, NULL);
                
                } else{
                    registrador(add,getReg(a3.contents.var.name),getReg(a3.contents.var.name),$lp);
                    imediato(str,getReg(a1.contents.var.name),getReg(a3.contents.var.name),aux,NULL);
                }
                if(switch_SO > QUANTUM && !SO) sistema(ctso, none);
                break;
            
            case opGOTO:
                salto(jmp, -1, a1.contents.var.name);
                break;
            
            case opRET:
                if (a1.kind == String) imediato(mov, $ret, getReg(a1.contents.var.name), 0, NULL);
                else imediato(mov, $ret, $zero, a1.contents.val, NULL);
                
                if(strcmp(a1.contents.var.scope,"main") != 0){
                    salto(jst, 0, NULL);

                } else salto(jmp, -1, "end");
                break;
            
            case opFUN:
                if (jmpmain == 0) {
                    salto(jmp, -1, "main");
                    jmpmain = 1;
                }
                insertLabel(a1.contents.var.name);
                Funcao(a1.contents.var.name);
                curarg = 0;
                break;
            
            case opEND:
                if (strcmp(a1.contents.var.name, "main") == 0) {
                    salto(jmp, -1, "end");
                }
                else {
                    salto(jst, 0, NULL);
                }
                break;
            
            case opPARAM:
                imediato(mov, getParamReg(), getReg(a1.contents.var.name), 0, NULL);
                curparam = (curparam+1)%nregparam;
                break;
            
            case opCALL:
                if (strcmp(a2.contents.var.name, "input") == 0) {
                    sistema(in, $ret);
                }
                else if (strcmp(a2.contents.var.name, "output") == 0) {
                    sistema(out,$p1);
                }
                else if (strcmp(a2.contents.var.name, "sysWake") == 0){
                    sistema(wake, none);
                }
                else if (strcmp(a2.contents.var.name, "sysSleep") == 0){
                    sistema(sleep, none);
                }
                else if (strcmp(a2.contents.var.name, "loadStack") == 0){
                    sistema(lstk, $p1);
                }
                else if (strcmp(a2.contents.var.name, "saveStack") == 0){
                    sistema(sstk, $p1);
                }
                else {
                    aux = getFunSize(a2.contents.var.scope);
                    imediato(addi,$lp,$lp,aux,NULL);
                    salto(jal, -1, a2.contents.var.name);
                    imediato(subi,$lp,$lp,aux,NULL);
                }
                narg = a3.contents.val;
                curparam = 0;
                break;
            
            case opARG:
                insertVar(a3.contents.var.scope, a1.contents.var.name, 1, checkType(l));
                FunList f = funlisthead;
                imediato(str, getArgReg(), $lp, getMemLoc(a1.contents.var.name,a1.contents.var.scope), NULL);
                curarg ++;
                break;
            
            case opLABEL:
                insertLabel(a1.contents.var.name);
                break;
            
            case opHALT:
                insertLabel("end");
                sistema(halt, none);
                break;
    
            default:
                break;
        }
        l = l->next;
        
    }
}

void criarInstrucao (QuadList head) {
    QuadList lista = head;
    inserirInstrucao(lista);
    AssemblyCode aux = codehead;
    while (aux != NULL) {
        if (aux->kind == instr) {
            switch(aux->line.instruction.opcode){ // Atualiza labels de desvios
            case jmp:
                aux->line.instruction.imed = getLabelLine(aux->line.instruction.label);
                break;
            case bne:
                aux->line.instruction.imed = getLabelLine(aux->line.instruction.label);
                break;
            case beq:
                aux->line.instruction.imed = getLabelLine(aux->line.instruction.label);
                break;
            case blt:
                aux->line.instruction.imed = getLabelLine(aux->line.instruction.label);
                break;
            case bgt:
                aux->line.instruction.imed = getLabelLine(aux->line.instruction.label);
                break;
            case jal:
                aux->line.instruction.imed = getLabelLine(aux->line.instruction.label);
                break;
            default:
                break;
            }
        }
        aux = aux->next;
    }
}

void printAssembly () {
    AssemblyCode a = codehead;
    if(PrintCode)
        printf( "\nC- Assembly Code\n" );
    while (a != NULL) {
        if (a->kind == instr) {
            switch(a->line.instruction.format){
            case formatR:
                fprintf(listing,"%s %s %s %s;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], regNames[a->line.instruction.reg2], regNames[a->line.instruction.reg3]);
                break;

            case formatI:
                if(a->line.instruction.opcode == sti || a->line.instruction.opcode == ldi)
                    fprintf(listing,"%s %s %d;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], a->line.instruction.imed);
                else if(a->line.instruction.opcode == put)
                    fprintf(listing,"%s %s %d;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], a->line.instruction.imed);
                else
                    fprintf(listing,"%s %s %s %d;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], regNames[a->line.instruction.reg2], a->line.instruction.imed);
            break;

            case formatSYS:
                if(a->line.instruction.opcode == halt || a->line.instruction.opcode == wake || a->line.instruction.opcode == sleep || a->line.instruction.opcode == ctso)
                    fprintf(listing,"%s;\n", InstrNames[a->line.instruction.opcode]);
                else if (a->line.instruction.opcode == in || a->line.instruction.opcode == out || a->line.instruction.opcode == sstk || a->line.instruction.opcode == lstk )
                    fprintf(listing,"%s %s;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg2]);
                else
                    fprintf(listing,"%s %s %d;\n", InstrNames[a->line.instruction.opcode], regNames[a->line.instruction.reg1], a->line.instruction.imed);
                break;

            case formatJ: {
                if (a->line.instruction.opcode == jst)
                    fprintf(listing,"%s;\n", InstrNames[a->line.instruction.opcode]);
                else
                    fprintf(listing,"%s %d;\n", InstrNames[a->line.instruction.opcode], a->line.instruction.imed);
            }
            }
        }
        else {
            fprintf(listing,"//%s\n", a->line.label);
        }
        a = a->next;
    }
    if(PrintCode)
        printf( "Código assembly criado com sucesso!\n");
}

/* Função inicial da geração de código assembly */
void codeass (QuadList head) {
    line = init_code;
    initCode(head);
    criarInstrucao(head);
    if(PrintCode)
        printAssembly();
   }

AssemblyCode getAssembly() {
    return codehead;
}
