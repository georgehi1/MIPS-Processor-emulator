#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "config.h"
#include "util.h"

// regs.c
int32_t regs(int write, int addr, int32_t val){
    static uint32_t regs[32] = {0};

    if (addr > 31)
        return -1; // This is really a silent error!

    if (write && addr > 0) // Don't allow write to zero reg
        regs[addr] = val;

    return regs[addr];
}

int32_t reg_r(int addr){
    return regs(0, addr, 0);
}

int reg_w(int addr, int32_t data){
    return regs(1, addr, data);
}

// bus.c
int bus(int write, uint32_t addr, int32_t* val){
    /*
     Memory Map:
        0x0000 0000: RAM
        ...
        0x0000 3fff: RAM
        0x0000 7f00: MMIO
            0x0000 7f00: Raw Output (8-bit)
            0x0000 7f04: Hex Output
            0x0000 7f08: Decimal Output
     */

    static uint32_t mem[RAM_SIZE/4];

    if (!write)
        *val = 0x0; // Bus default to 0x0 on read

    if (addr < RAM_SIZE){
        if (write)
            mem[addr/4] = *val;
        else
            *val = mem[addr/4];
    } else if (addr == 0x7f00){
        if (write)
            printf("%c", *val & 0xff);
    } else if (addr == 0x7f04){
        if (write)
            printf("0x%08x\n", *val);
    } else if (addr == 0x7f08){
        if (write)
            printf("%ld\n", (long) *val);
    } else {
        fprintf(stderr, "\n--------\nINVALID MEMORY ACCESS (0x%04x). EXITING.\n", addr);
        exit(0);
    }

    return 0;
}

// loadmem.c
int loadmem(const char* filename, uint32_t offset){
    FILE *fp;
    int i, n;
    int32_t data = 0x0;

    fp = fopen(filename, "rb");
    if (fp == NULL)
        return -1;

    i = 0;
    while ((n = fread(&data, 1, 4, fp))){
        bus(1, offset+i, &data);
        data = 0x0;

        i += n;
        if (i >= RAM_SIZE)
            break;
    }

    fclose(fp);

    return i;
}

// test.c
void cycle(void){
    static int i = 0;
    i++;

    if (i >= CYCLE_LIMIT)
        exit(1);
}
