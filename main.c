#include <stdio.h>
#include <stdint.h>

#include "config.h"
#include "util.h"

void fetch();
void decode();
void execute();
void memory();
void writeback();

int32_t pc;
int32_t _bus;

int8_t op, rs, rt, rd, shamt, funct;
int32_t imm;
int32_t jaddr;

int pc_jmp, pc_src;
int reg_dst;
int reg_wb;

int32_t alu_a, alu_b;
enum {ADD, SUB, AND, OR, XOR, NOR, LW, SW, JR, SLT, SLLV, BEQ, BNE, SRLV} alu_op;
int32_t alu_out;
int alu_zero;

int mem_en;
int mem_wen;
int32_t mem_wdata;
int mem_to_reg;

int main(int argc, char **argv){
    int n;

    // Read in memory files, if set
    if (argc > 1){
        fprintf(stderr, "Loading RAM .text(0x0000) from '%s'... ", argv[1]);
        n = loadmem(argv[1], 0x0);
        if (n >= 0)
            fprintf(stderr, "%d bytes.\n", n);
        else
            fprintf(stderr, "ERROR.\n");

        if (argc > 2){
            fprintf(stderr, "Loading RAM .data(0x2000) from '%s'... ", argv[2]);
            n = loadmem(argv[2], 0x2000);
            if (n >= 0)
                fprintf(stderr, "%d bytes.\n", n);
            else
                fprintf(stderr, "ERROR.\n");
        }
    }

    // CPU Loop
    while (1){
        fetch();
        decode();
        execute();
        memory();
        writeback();
        
		if(pc_jmp){
			pc = ((pc + 4) & 0xf0000000) | (jaddr << 2);
		}else{
            pc = pc+4;
        }

        /*
         cycle() will kill your simulator after 4096 cycles to prevent reading
            outside of RAM.
         If you want to run for longer, change your CYCLE_LIMIT in config.h.*/
        
        cycle(); 
    }
}

void fetch(){
    bus(0, pc, &_bus);
}

void decode(){
    op = (_bus >> 26) & 0x3f; // top 6 bits of instruction
    rs = (_bus >> 21) & 0x1f; // bits 21-25
    rt = (_bus >> 16) & 0x1f; // bits 16-20
    rd = (_bus >> 11) & 0x1f; // bits 11-15
    imm = (_bus << 16) >> 16; // ??? = bottom 16 bits of instruction, remember to sign extend!
    shamt = 0x0; // Not needed for coursework
	funct = _bus & 0x3f; // bottom 6 bits of instruction

    jaddr = _bus & 0x3ffffff; // bottom 26 bits of instruction, no need to sign extend since it will be automatically handled when jumping

    // Default output signals
    pc_jmp = 0;
    pc_src = 0;
    reg_dst = 0;
    reg_wb = 0;
    mem_en = 0;
    mem_wen = 0;
    mem_to_reg = 0;

    alu_op = ADD;
    alu_a = reg_r(rs);
    alu_b = reg_r(rt);

   switch (op){
        case 0x00: // R-type
            reg_dst = 1; // writeback will go to rd
            reg_wb = 1;
            break;
        case 0x08: // ADDI
            alu_op = ADD;
            alu_b = imm;
            reg_wb = 1;
            break;
        case 0x23: // LW
			alu_op = LW;
			mem_en = 1;
			mem_to_reg = 1; 
			reg_wb = 1;     
        case 0x2B: // SW 
            alu_op = SW;
            mem_en = 1;
            mem_wen = 1;
            break;
        case 0x04: // BEQ
            alu_op = BEQ;
            pc_src = 1;
            break;
        case 0x05: // BNE
            alu_op = BNE;
            pc_src = 1;
            break;
         case 0x02: // J
            pc_jmp = 1;
            break;

     }


   if (op == 0x00){
        switch (funct){
            case 0x20: // ADD
                alu_op = ADD;
                break;
            case 0x22: // SUB
                alu_op = SUB;
                break;
            case 0x24: // AND
                alu_op = AND;
                break;
            case 0x25: // OR
                alu_op = OR;
                break;
            case 0x26: // XOR
                alu_op = XOR;
                break;
            case 0x27: // NOR
                alu_op = NOR;
                break;
            case 0x08: //JR
                alu_op = JR;
                pc_jmp = 1;
                break;
            case 0x2A: // SLT
                alu_op = SLT;
                break;
            case 0x04: // SLLV
                alu_op = SLLV;
                reg_wb = 1;
                break;
            case 0x06: // SRLV 
                alu_op = SRLV;
                reg_wb =1;
                break;              
          }
      }
    }


void execute(){
    
    switch (alu_op){
        case ADD:
            alu_out = alu_a + alu_b;
            break;
        case SUB:
            alu_out = alu_a - alu_b;
            break;
        case SW:
            alu_out = alu_a + imm;
            break;
        case LW:
            alu_out = alu_a + imm;
            break;
        case AND:
            alu_out = alu_a & alu_b;
            break;
        case OR:
            alu_out = alu_a | alu_b;
            break;
        case XOR:
            alu_out = alu_a ^ alu_b;
            break;
        case NOR:
            alu_out = ~(alu_a | alu_b);
            break;
        case JR:
            pc = alu_a;
            break;
        case BEQ:
            if(alu_a == alu_b){
                pc = pc + (imm<<2);
            }
            break;
        case BNE:
            if(alu_a != alu_b){
                pc = pc + (imm<<2);
            }
            break;
        case SLT:
            alu_out = (alu_a < alu_b) ? 1 : 0;
            break;
        case SLLV:
            alu_out = alu_b << alu_a;
            break;
        case SRLV:
            alu_out = alu_b >> alu_a;
            break;

        alu_zero = alu_out == 0 ? 1 : 0;
    }
}


void memory(){
    if (!mem_en)
        return;
        if (alu_op == SW) {
            //mem_wen is 1 so sets bus into write mode
            bus(mem_wen, alu_out, &alu_b);
        } 
    
}

void writeback(){
    if (reg_wb)
        reg_w(reg_dst ? rd : rt, mem_to_reg ? _bus : alu_out);
}
