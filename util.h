#include <stdint.h>

#ifndef CYCLE_LIMIT
#define CYCLE_LIMIT 4096
#endif
#ifndef RAM_SIZE
#define RAM_SIZE 0x4000
#endif

// regs.h
int32_t regs(int write, int addr, int32_t val);
int32_t reg_r(int addr);
int reg_w(int addr, int32_t data);

// bus.h
int bus(int write, uint32_t addr, int32_t* val);

// loadmem.h
int loadmem(const char* filename, uint32_t offset);

// test.h
void cycle(void);
