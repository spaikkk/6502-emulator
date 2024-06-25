#include <stdio.h>
#include <stdint.h>
#define ROM_SIZE 0xBFDF
#define RAM_SIZE 0x4019
#define ROM_OFFSET 0x4020
#define RAM_MODULE 0x7FF
#define STACK_OFFSET 0x100
typedef struct{
    uint8_t ram[RAM_SIZE];
    uint8_t rom[ROM_SIZE];
}Memory;

typedef struct{
    uint8_t c; //carry (bit 0)
    uint8_t z; //zero (bit 1)
    uint8_t i; //interrupt (bit 2)
    uint8_t b; //break (bit 4)
    uint8_t v; //overflow (bit 6)
    uint8_t n; //negative (bit 7)
}statusRegister;

typedef struct{
    uint16_t pc; //program counter
    uint8_t a; //accumulator
    uint8_t x; //X register
    uint8_t y; //y register
    statusRegister p; //status register
    uint8_t s; //stack pointer
}Registers;

typedef struct{
    Registers regs;
    Memory mem;
    unsigned int cycles;
}cpu_6502;

void cpu_tick(cpu_6502 *cpu_6502);
void cpu_init(cpu_6502 *cpu);

//access to memory functions
uint8_t isRom(uint16_t address);
uint8_t isMirror(uint16_t address);
uint8_t readMemory(cpu_6502 *cpu_6502, uint16_t address);
void writeMemory(cpu_6502 *cpu_6502, uint16_t address, uint8_t data);

//access to stack
void push_s(cpu_6502 *cpu_6502, uint8_t data);
uint8_t pull_s(cpu_6502 *cpu_6502);

//addressing modes
void implied(cpu_6502 *cpu_6502);
uint16_t immediate(cpu_6502 *cpu_6502);
uint16_t absolute(cpu_6502 *cpu_6502);
uint16_t zeroPage(cpu_6502 *cpu_6502, uint8_t XY_value);
uint16_t absolute_indexed(cpu_6502 *cpu_6502, uint8_t indexValue);
signed char relative(cpu_6502 *cpu_6502);
uint16_t preIndexed_indirect(cpu_6502 *cpu_6502);
uint16_t postIndexed_indirect(cpu_6502 *cpu_6502);
uint16_t indirect_absolute(cpu_6502 *cpu_6502);

//utilities
int sign(signed char num);
void printFlagReg(Registers *regs);
void load(uint8_t valore, uint8_t *registro);
void setZNflag(cpu_6502 *cpu_6502, uint8_t operand);

//operations
void inst_adc(cpu_6502 *cpu_6502, uint16_t opAddress);
void inst_and(cpu_6502 *cpu_6502, uint16_t opAddress);
void inst_asl(cpu_6502 *cpu_6502, uint16_t opAddress);
void inst_asl_ac(cpu_6502 *cpu_6502);
void isnt_bcc(cpu_6502 *cpu_6502, int8_t offset);
void inst_bcs(cpu_6502 *cpu_6502, int8_t offset);
void inst_beq(cpu_6502 *cpu_6502, int8_t offset);
void inst_bit(cpu_6502 *cpu_6502, uint16_t op_reg);
void inst_bmi(cpu_6502 *cpu_6502, int8_t offset);
void inst_bne(cpu_6502 *cpu_6502, int8_t offset);
void inst_bpl(cpu_6502 *cpu_6502, int8_t offset);
void inst_brk(cpu_6502 *cpu_6502);
void inst_bvc(cpu_6502 *cpu_6502, int8_t offset);
void inst_bvs(cpu_6502 *cpu_6502, int8_t offset);
void inst_clc(cpu_6502 *cpu_6502);
void inst_cli(cpu_6502 *cpu_6502);
void inst_clv(cpu_6502 *cpu_6502);
void inst_cmp(cpu_6502 *cpu_6502, uint16_t op_reg);
void inst_cpx(cpu_6502 *cpu_6502, uint16_t op_reg);
void inst_cpy(cpu_6502 *cpu_6502, uint16_t op_reg);
void inst_dec(cpu_6502 *cpu_6502, uint16_t memLoc);
void inst_dex(cpu_6502 *cpu_6502);
void inst_dey(cpu_6502 *cpu_6502);
void inst_eor(cpu_6502 *cpu_6502, uint16_t op_reg);
void inst_inc(cpu_6502 *cpu_6502, uint16_t memLoc);
void inst_inx(cpu_6502 *cpu_6502);
void inst_iny(cpu_6502 *cpu_6502);
void inst_abs_jmp(cpu_6502 *cpu_6502, uint16_t jmp_addr);
void inst_ind_jmp(cpu_6502 *cpu_6502);
void inst_jsr(cpu_6502 *cpu_6502);
void inst_lda(cpu_6502 *cpu_6502, uint16_t op_addr);
void inst_ldx(cpu_6502 *cpu_6502, uint16_t op_addr);
void inst_ldy(cpu_6502 *cpu_6502, uint16_t op_addr);
void inst_lsr(cpu_6502 *cpu_6502, uint16_t mem_addr);
void inst_lsr_a(cpu_6502 *cpu_6502);
void inst_nop();
void inst_ora(cpu_6502 *cpu_6502, uint16_t op_addr);
void inst_pha(cpu_6502 *cpu_6502);
void isnt_php(cpu_6502 *cpu_6502);
void inst_pla(cpu_6502 *cpu_6502);
void inst_plp(cpu_6502 *cpu_6502);
void inst_rol(cpu_6502 *cpu_6502, uint16_t mem_addr);
void inst_rol_a(cpu_6502 *cpu_6502);
void inst_ror(cpu_6502 *cpu_6502, uint16_t mem_addr);
void inst_ror_a(cpu_6502 *cpu_6502);
void inst_rti(cpu_6502 *cpu_6502);
void inst_rts(cpu_6502 *cpu_6502);
void inst_sbc(cpu_6502 *cpu_6502, uint16_t op_address);
void inst_sec(cpu_6502 *cpu_6502);
void inst_sei(cpu_6502 *cpu_6502);
void inst_sta(cpu_6502 *cpu_6502, uint16_t mem_addr);
void inst_stx(cpu_6502 *cpu_6502, uint16_t mem_addr);
void inst_sty(cpu_6502 *cpu_6502, uint16_t mem_addr);
void inst_tax(cpu_6502 *cpu_6502);
void inst_tay(cpu_6502 *cpu_6502);
void inst_tsx(cpu_6502 *cpu_6502);
void inst_txa(cpu_6502 *cpu_6502);
void inst_txs(cpu_6502 *cpu_6502);
void inst_tya(cpu_6502 *cpu_6502);