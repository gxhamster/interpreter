#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define CHECK_STACK_BOUND(p) if (p > (stack + poolsize) || p < stack) { vm_clean(); }
#define CHECK_PC_BOUND(p) if (p > (text + poolsize) || p < text) { vm_clean(); }

typedef unsigned int u_int;

// Memory regions
int *text;
int *stack;
char *data;

int poolsize = 8 * 1000;

// Pointers
int *pc, *bp, *sp;
int ax, cycle;

// opcodes
// TODO: Please remember to put the new OPCODE in all the necessary places
enum { IMM, LC, LI, SC, SI, PUSH, ADD, SUB, JMP, EXIT, PRINT };

int init_memory()
{
    // Allocate memory regions
    if (!(text = malloc(poolsize * sizeof(int)))) {
        fprintf(stderr, "ERROR: could not allocate text area\n");
        return -1;
    }
    if (!(data = malloc(poolsize))) {
        fprintf(stderr, "ERROR: could not allocate data area\n");
        return -1;
    }
    if (!(stack = malloc(poolsize * sizeof(int)))) {
        fprintf(stderr, "ERROR: could not allocate stack area\n");
        return -1;
    }

    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize);

    // Stack items increase from high to low memory address
    bp = sp = (int *)(stack + poolsize);
    ax = 0;

    return 0;
}

void vm_clean()
{
    free(text);
    free(data);
    free(stack);
    exit(-1);
}

void eval()
{
    int op;
    while (1) {
        CHECK_PC_BOUND(pc+1);
        op = *pc++;
        switch(op) {
            case IMM:
                ax = *pc++;
                break;
            case LC:
                ax = *(char *)ax;
                break;
            case LI:
                ax = *(int *)ax;
                break;
            case SC:
                CHECK_STACK_BOUND(sp+1);
                *(char *)*sp++ = ax;
                break;
            case SI:
                CHECK_STACK_BOUND(sp+1);
                *(int *)*sp++ = ax;
                break;
            case PUSH:
                CHECK_STACK_BOUND(sp-1);
                *--sp = ax;
                break;
            case ADD:
                CHECK_STACK_BOUND(sp+1);
                ax = *sp++ + ax;
                break;
            case SUB:
                CHECK_STACK_BOUND(sp+1);
                ax = *sp++ - ax;
                break;
            case JMP:
                // Right now JMP uses an offset from the starting point (not actual address)
                pc = text + *pc;
                break;
            case PRINT:
                printf("print: %d\n", ax);
                break;
            case EXIT:
                printf("exit(%d)\n", *sp); 
                return;
            default:
                continue;
        }
    }
}

typedef struct {
    char *key;
    int value;
} symstruct;
int lookup_op(char *str)
{
    symstruct lookup_table[] = {
        {"IMM", IMM},
        {"LC", LC},
        {"LI", LI},
        {"SC", SC},
        {"SI", IMM},
        {"PUSH", PUSH},
        {"ADD", ADD},
        {"SUB", SUB},
        {"JMP", JMP},
        {"EXIT", EXIT},
        {"PRINT", PRINT},
    };
    int lookup_table_size = sizeof(lookup_table) / sizeof(symstruct);
    int i;
    for (i = 0; i < lookup_table_size; i++) {
        if (strcmp(str, lookup_table[i].key) == 0) {
            return lookup_table[i].value;
        }
    }

    return -1;
}

void read_op_from_file(FILE *fp)
{
    if (fp == NULL)
        return;

    size_t bufsize = 100; 
    char buf[bufsize];
    int i = 0;
    int buf_op;
    // enum { IMM, LC, LI, SC, SI, PUSH, ADD, SUB, EXIT };
    while (fgets(buf, bufsize, fp)) {
        buf[strcspn(buf, "\n")] = 0;
        // ignore comments
        if (strstr(buf, "//")) {
            continue;
        }
        buf_op = lookup_op(buf);
        switch (buf_op) {
            case IMM:
                text[i++] = IMM;
                break;
            case LC:
                text[i++] = LC;
                break;
            case LI:
                text[i++] = LI;
                break;
            case SC:
                text[i++] = SC;
                break;
            case SI:
                text[i++] = SI;
                break;
            case PUSH:
                text[i++] = PUSH;
                break;
            case ADD:
                text[i++] = ADD;
                break;
            case SUB:
                text[i++] = SUB;
                break;
            case JMP:
                text[i++] = JMP;
                break;
            case PRINT:
                text[i++] = PRINT;
                break;
            case EXIT:
                text[i++] = EXIT;
                break;
            default:
                // TODO: handle data
                text[i++] = atoi(buf);
                break;
        }
        // printf("%s ", buf);
    }
}

int main()
{
    // Initlaize VM
    init_memory();

    FILE *fp;
    fp = fopen("program.vm", "r");
    read_op_from_file(fp);
    pc = text;

    eval();
}
