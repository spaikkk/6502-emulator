#include "functions.h"

/*REMINDER
    always set a tick when you read opcode in main function
*/
void cpu_tick(cpu_6502 *cpu_6502){
    cpu_6502->cycles++;
}
cpu_init(cpu_6502 *cpu){
	cpu->regs.a = 0;
    cpu->regs.x = 0;
    cpu->regs.y = 0;
    cpu->regs.pc = 0xFFFC;
    cpu->regs.s = 0xFD;
    cpu->regs.p.c = 0;
    cpu->regs.p.z = 0;
    cpu->regs.p.i = 1;
    cpu->regs.p.v = 0;
    cpu->regs.p.n = 0;
	cpu->cycles = 0;

}
uint8_t isRom(uint16_t address){
	uint8_t check = address>=0x4020;
	return check;
};

uint8_t isMirror(uint16_t address){
    uint8_t check = address>=0x800&&address<=0x1FFF;
    return check;
}


uint8_t readMemory(cpu_6502 *cpu_6502, uint16_t address) {
    return isRom(address) ? cpu_6502->mem.rom[address - ROM_OFFSET] :
           isMirror(address) ? cpu_6502->mem.ram[address & RAM_MODULE] :
           cpu_6502->mem.ram[address];
}

void writeMemory(cpu_6502 *cpu_6502, uint16_t address, uint8_t data){
    if(isMirror(address)){
        cpu_6502->mem.ram[address&RAM_MODULE]=data;
    }else{
        cpu_6502->mem.ram[address]=data;
    }
}

void push_s(cpu_6502 *cpu_6502, uint8_t data){
    uint16_t stack_ptr = (cpu_6502->regs.s | STACK_OFFSET);
    writeMemory(cpu_6502, stack_ptr, data);
    cpu_6502->regs.s--;
    cpu_tick(cpu_6502); 
}

uint8_t pull_s(cpu_6502 *cpu_6502){
    /*stack pointer increment needs to be done manually in the function
      where the pull il called, bc with pipelining the increment
      is handled differently depending on the case
    */
    uint16_t stack_ptr = (cpu_6502->regs.s | STACK_OFFSET);
    uint8_t data = readMemory(cpu_6502, stack_ptr);
    //cpu_tick(cpu_6502); needs to be called in function
    return data;
}


////////////////////
//ADDRESSING MODES//
////////////////////
void implied(cpu_6502 *cpu_6502){
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502); //read the instruction byte after opcode and throw it away
}
//pc increment (first tick) needs to be done in main after fetching upcode
uint16_t immediate(cpu_6502 *cpu_6502){
    return cpu_6502->regs.pc;
}
//done
uint16_t absolute(cpu_6502 *cpu_6502){
    uint16_t address;
    address = readMemory(cpu_6502, cpu_6502->regs.pc);
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502); //tick 2
    address|=readmemory(cpu_6502, cpu_6502->regs.pc)<<8;
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502);
    return address;
}
//done
uint16_t zeroPage(cpu_6502 *cpu_6502, uint8_t XY_value){
    uint16_t address = readMemory(cpu_6502, cpu_6502->regs.pc);
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502);
    if(XY_value!=0){
        address+=XY_value;
        cpu_tick(cpu_6502);
    }
    return address&0xFF;
}

uint16_t absolute_indexed(cpu_6502 *cpu_6502, uint8_t indexValue){
    uint8_t lowByte = readMemory(cpu_6502, cpu_6502->regs.pc);
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502);
    uint8_t highByte = readMemory(cpu_6502, cpu_6502->regs.pc);
    cpu_6502->regs.pc++;
    uint16_t address = (lowByte|(highByte<<8))+indexValue;
    cpu_tick(cpu_6502);
    if ((lowByte+indexValue)>0xFF){
        cpu_tick(cpu_6502); //page boundaries crossed
    }
    return address;
}

//returns only the offset as an unsigned char
signed char relative(cpu_6502 *cpu_6502){
    uint8_t offset = readMemory(cpu_6502, cpu_6502->regs.pc);
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502);
    return offset;
}

uint16_t preIndexed_indirect(cpu_6502 *cpu_6502){
    uint8_t pointerAddress = readMemory(cpu_6502, cpu_6502->regs.pc);
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502); //fetch pointer address
    pointerAddress += cpu_6502->regs.x;
    cpu_tick(cpu_6502); //add x to it
    uint16_t effectiveAddress = readMemory(cpu_6502, pointerAddress); //lowByte
    cpu_tick(cpu_6502);//fetch efffective address low
    pointerAddress++;
    effectiveAddress |= readMemory(cpu_6502, pointerAddress)<<8; //highByte
    cpu_tick(cpu_6502); //fetch effective address high
    return effectiveAddress;
}


uint16_t postIndexed_indirect(cpu_6502 *cpu_6502){
    uint8_t pointerAddress = readMemory(cpu_6502, cpu_6502->regs.pc);
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502); //fetch pointer address
    uint8_t lowByte = readMemory(cpu_6502, pointerAddress);
    cpu_tick(cpu_6502); //fetch effective addres lowByte
    pointerAddress++;
    uint8_t highByte = readMemory(cpu_6502, pointerAddress);
    uint16_t effectiveAddress = (lowByte|(highByte<<8))+cpu_6502->regs.y;
    cpu_tick(cpu_6502);
    if (lowByte+cpu_6502->regs.y>0xFF){
        cpu_tick(cpu_6502);
    }
    return effectiveAddress;
}

uint16_t indirect_absolute(cpu_6502 *cpu_6502){
    uint16_t pointerAddress = readMemory(cpu_6502, cpu_6502->regs.pc); //pointer add. low
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502);
    pointerAddress |= readMemory(cpu_6502, cpu_6502->regs.pc)<<8;
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502);
    uint16_t returnAddress = readMemory(cpu_6502, pointerAddress); //effective low
    cpu_tick(cpu_6502);
    pointerAddress++;
    returnAddress |= readMemory(cpu_6502, pointerAddress); //effective high
    cpu_tick(cpu_6502);

    return returnAddress;
}

//////////////////////
//    OPERATIONS    //
//////////////////////
int sign(signed char num){
    if(num>=0){
        return 1;
    }else{
        return 0;
    }
}
void printFlagReg(Registers *regs){
    printf("%d Negative\n%d Zero\n%d Overflow\n%d Carry\n%d Break\n%d Interrupt\n", 
    regs->p.n, regs->p.z, regs->p.v, regs->p.c, regs->p.b, regs->p.i);
}

void load(uint8_t valore, uint8_t *registro){
    *registro = valore;
}
void setZNflag(cpu_6502 *cpu_6502, uint8_t operand){
    cpu_6502->regs.p.n = (operand & 0b10000000)>>7;
    cpu_6502->regs.p.z = (operand==0) ? 1 : 0;
}


//add with carry
void inst_adc(cpu_6502 *cpu_6502, uint16_t opAddress){
    uint8_t operand = readMemory(cpu_6502, opAddress);
    cpu_tick(cpu_6502);

    unsigned int valueInt = operand + cpu_6502->regs.a + cpu_6502->regs.p.c;
    char var1 = (char)cpu_6502->regs.a;
    char var2 = (char)operand;
    char valueSigned = var1 + var2;
    //check for overflow or carry
    cpu_6502->regs.p.c = (valueInt>255);
    cpu_6502->regs.p.v = !(sign(cpu_6502->regs.a)^sign(operand))&&
    (sign(valueSigned!=sign(operand)));
    cpu_6502->regs.a = (uint8_t)valueInt; //store the result
    //set remaining flags
    setZNflag(cpu_6502, cpu_6502->regs.a);
}

//and with accumulator
void inst_and(cpu_6502 *cpu_6502, uint16_t opAddress){
    uint8_t operand = readMemory(cpu_6502, opAddress);
    cpu_tick(cpu_6502);

    cpu_6502->regs.a &= operand;
    setZNflag(cpu_6502, cpu_6502->regs.a);
}

//arithmetic shift left
void inst_asl(cpu_6502 *cpu_6502, uint16_t opAddress){
    uint8_t data = readMemory(cpu_6502, opAddress);
    cpu_tick(cpu_6502);
    cpu_6502->regs.p.c = (data>=128);
    data = data<<1;
    setZNflag(cpu_6502, data);
    cpu_tick(cpu_6502);
    writeMemory(cpu_6502, opAddress, data);
    cpu_tick(cpu_6502);
}
//arithmetic shift left accumulator
void inst_asl_ac(cpu_6502 *cpu_6502){
    cpu_6502->regs.p.c = (cpu_6502->regs.a>=128);
    cpu_6502->regs.a << 1;
    setZNflag(cpu_6502, cpu_6502->regs.a);
}

//branch if carry clear
void isnt_bcc(cpu_6502 *cpu_6502, int8_t offset){
    uint8_t pcLowByte = cpu_6502->regs.pc;
    if(!cpu_6502->regs.p.c){
        cpu_6502->regs.pc += offset;
    }else{
        cpu_6502->regs.pc++;
    }
    cpu_tick(cpu_6502);

    if((pcLowByte+offset)>0xFF){
        cpu_tick(cpu_6502); //page boundaries crossed
    }
}

//branch if carry set
void inst_bcs(cpu_6502 *cpu_6502, int8_t offset){
    uint8_t pcLowByte = cpu_6502->regs.pc;
    if(cpu_6502->regs.p.c){
        cpu_6502->regs.pc += offset;
    }else{
        cpu_6502->regs.pc++;
    }
    cpu_tick(cpu_6502);

    if((pcLowByte+offset)>0xFF){
        cpu_tick(cpu_6502); //page boundaries crossed
    }
}

//branch if equal
void inst_beq(cpu_6502 *cpu_6502, int8_t offset){
    uint8_t pcLowByte = cpu_6502->regs.pc;
    if(cpu_6502->regs.p.z){
        cpu_6502->regs.pc += offset;
    }else{
        cpu_6502->regs.pc++;
    }
    cpu_tick(cpu_6502);

    if((pcLowByte+offset)>0xFF){
        cpu_tick(cpu_6502); //page boundaries crossed
    }
}

//test memory bits
void inst_bit(cpu_6502 *cpu_6502, uint16_t op_reg){
    uint8_t m = readMemory(cpu_6502, op_reg);
    cpu_tick(cpu_6502);

    cpu_6502->regs.p.n = (m & 0x80)>>7;
    cpu_6502->regs.p.v = (m & 0x40)>>6;
    cpu_6502->regs.p.z = !(m & cpu_6502->regs.a);
}

//branch if minus
void inst_bmi(cpu_6502 *cpu_6502, int8_t offset){
    uint8_t pcLowByte = cpu_6502->regs.pc;
    if(cpu_6502->regs.p.n){
        cpu_6502->regs.pc += offset;
    }else{
        cpu_6502->regs.pc++;
    }
    cpu_tick(cpu_6502);

    if((pcLowByte+offset)>0xFF){
        cpu_tick(cpu_6502); //page boundaries crossed
    }
}

//branch if not equal
void inst_bne(cpu_6502 *cpu_6502, int8_t offset){
    uint8_t pcLowByte = cpu_6502->regs.pc;
    if(!cpu_6502->regs.p.z){
        cpu_6502->regs.pc += offset;
    }else{
        cpu_6502->regs.pc++;
    }
    cpu_tick(cpu_6502);

    if((pcLowByte+offset)>0xFF){
        cpu_tick(cpu_6502); //page boundaries crossed
    }
}

//branch if result positive
void inst_bpl(cpu_6502 *cpu_6502, int8_t offset){
    uint8_t pcLowByte = cpu_6502->regs.pc;
    if(!cpu_6502->regs.p.n){
        cpu_6502->regs.pc += offset;
    }else{
        cpu_6502->regs.pc++;
    }
    cpu_tick(cpu_6502);

    if((pcLowByte+offset)>0xFF){
        cpu_tick(cpu_6502); //page boundaries crossed
    }
}



//OPCODE = BRK -> tick updated
void inst_brk(cpu_6502 *cpu_6502){
    /*when the function is called the pc counter has already been 
    incremented by 1 while reading the opcode and another 1 after
    reading the extra instruction byte from implied addressing
    */
    cpu_6502->regs.p.b = 1;
    uint8_t pcH = cpu_6502->regs.pc>>8, pcL = cpu_6502->regs.pc;
    push_s(cpu_6502, pcH); //push PCH on stack, decrement Sp
    push_s(cpu_6502, pcL); //push PCL on stack, decrement Sp
    
    uint8_t srByte = 0;
    srByte |= cpu_6502->regs.p.c; //bit 0
    srByte |= cpu_6502->regs.p.z << 1; //bit 1
    srByte |= cpu_6502->regs.p.i << 2; //bit 2
    srByte |= cpu_6502->regs.p.b << 4; //bit 4
    srByte |= cpu_6502->regs.p.v << 6; //bit 6
    srByte |= cpu_6502->regs.p.n << 7; //bit 7

    push_s(cpu_6502, srByte); //push P on stack, decrement s
    cpu_6502->regs.p.i = 1;
    
    cpu_6502->regs.pc = readMemory(cpu_6502, 0XFFFE);
    cpu_tick(cpu_6502); //fetch PCL
    cpu_6502->regs.pc |= readMemory(cpu_6502, 0xFFFF)<<8;
    cpu_tick(cpu_6502); //fetch pch
}

//branch if overflow clear
void inst_bvc(cpu_6502 *cpu_6502, int8_t offset){
    uint8_t pcLowByte = cpu_6502->regs.pc;
    if(!cpu_6502->regs.p.v){
        cpu_6502->regs.pc += offset;
    }else{
        cpu_6502->regs.pc++;
    }
    cpu_tick(cpu_6502);

    if((pcLowByte+offset)>0xFF){
        cpu_tick(cpu_6502); //page boundaries crossed
    }
}

//branch if overflow set
void inst_bvs(cpu_6502 *cpu_6502, int8_t offset){
    uint8_t pcLowByte = cpu_6502->regs.pc;
    if(cpu_6502->regs.p.v){
        cpu_6502->regs.pc += offset;
    }else{
        cpu_6502->regs.pc++;
    }
    cpu_tick(cpu_6502);

    if((pcLowByte+offset)>0xFF){
        cpu_tick(cpu_6502); //page boundaries crossed
    }
}

//clear carry flag
void inst_clc(cpu_6502 *cpu_6502){
    cpu_6502->regs.p.c = 0;
}

//clear interrupt disable flag
void inst_cli(cpu_6502 *cpu_6502){
    cpu_6502->regs.p.i = 0;
}

//clear overflow flag
void inst_clv(cpu_6502 *cpu_6502){
    cpu_6502->regs.p.v = 0;
}

//compare memory with accumulator
void inst_cmp(cpu_6502 *cpu_6502, uint16_t op_reg){
    uint8_t mem = readMemory(cpu_6502, op_reg);
    cpu_tick(cpu_6502);
    uint8_t cmp = cpu_6502->regs.a - mem;
    setZNflag(cpu_6502, cmp);
    cpu_6502->regs.p.c = (cpu_6502->regs.a>=mem);
}

//compare memory and index X
void inst_cpx(cpu_6502 *cpu_6502, uint16_t op_reg){
    uint8_t mem = readMemory(cpu_6502, op_reg);
    cpu_tick(cpu_6502);
    uint8_t cmp = cpu_6502->regs.x - mem;
    setZNflag(cpu_6502, cmp);
    cpu_6502->regs.p.c = (cpu_6502->regs.x>=mem);
}

//compare memory and index y
void inst_cpy(cpu_6502 *cpu_6502, uint16_t op_reg){
    uint8_t mem = readMemory(cpu_6502, op_reg);
    cpu_tick(cpu_6502);
    uint8_t cmp = cpu_6502->regs.y - mem;
    setZNflag(cpu_6502, cmp);
    cpu_6502->regs.p.c = (cpu_6502->regs.y>=mem);
}

//decrement memory by one
void inst_dec(cpu_6502 *cpu_6502, uint16_t memLoc){
    uint8_t mem = readMemory(cpu_6502, memLoc);
    cpu_tick(cpu_6502);
    mem--;
    setZNflag(cpu_6502, mem);
    cpu_tick(cpu_6502);
    writeMemory(cpu_6502, memLoc, mem);
    cpu_tick(cpu_6502);
}

//decrement x 
void inst_dex(cpu_6502 *cpu_6502){
    cpu_6502->regs.x--;
    setZNflag(cpu_6502, cpu_6502->regs.x);
}

//decrement y
void inst_dey(cpu_6502 *cpu_6502){
    cpu_6502->regs.y--;
    setZNflag(cpu_6502, cpu_6502->regs.y);
}

//exclusive or memory with accumulator
void inst_eor(cpu_6502 *cpu_6502, uint16_t op_reg){
    uint8_t mem = readMemory(cpu_6502, op_reg);
    cpu_tick(cpu_6502);
    cpu_6502->regs.a ^= mem;
    setZNflag(cpu_6502, cpu_6502->regs.a);
}

//increment memory by one
void inst_inc(cpu_6502 *cpu_6502, uint16_t memLoc){
    uint8_t mem = readMemory(cpu_6502, memLoc);
    cpu_tick(cpu_6502);
    mem++;
    setZNflag(cpu_6502, mem);
    cpu_tick(cpu_6502);
    writeMemory(cpu_6502, memLoc, mem);
    cpu_tick(cpu_6502);
}

//increment x
void inst_inx(cpu_6502 *cpu_6502){
    cpu_6502->regs.x++;
    setZNflag(cpu_6502, cpu_6502->regs.x);
}

//increment y 
void inst_iny(cpu_6502 *cpu_6502){
    cpu_6502->regs.y++;
    setZNflag(cpu_6502, cpu_6502->regs.y);
}

//jump absolute 
void inst_abs_jmp(cpu_6502 *cpu_6502, uint16_t jmp_addr){
    cpu_6502->regs.pc = jmp_addr;
}   
//jump indirect, in main no need for addressing type function
void inst_ind_jmp(cpu_6502 *cpu_6502){
    uint16_t pointerAddress = readMemory(cpu_6502, cpu_6502->regs.pc); //pointer add. low
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502);
    pointerAddress |= readMemory(cpu_6502, cpu_6502->regs.pc)<<8; //pointer address high
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502);
    cpu_6502->regs.pc = readMemory(cpu_6502, pointerAddress); //effective low
    cpu_tick(cpu_6502);
    pointerAddress++;
    cpu_6502->regs.pc |= readMemory(cpu_6502, pointerAddress)<<8; //effective high
    cpu_tick(cpu_6502);
}

//jump to subroutine - no addressing in main
void inst_jsr(cpu_6502 *cpu_6502){
    uint16_t jmp_addr = readMemory(cpu_6502, cpu_6502->regs.pc); //fetch low byte
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502);
    cpu_tick(cpu_6502);//internal operation
    uint8_t pcL = cpu_6502->regs.pc, pcH = cpu_6502->regs.pc>>8;
    push_s(cpu_6502, pcH);//push pch, tick
    push_s(cpu_6502, pcL);//push pcl, tick
    jmp_addr |= readMemory(cpu_6502, cpu_6502->regs.pc)<<8; //fetch high byte
    cpu_6502->regs.pc = jmp_addr;
    cpu_tick(cpu_6502); //copy operand to pc
}

//load accumulator with memory
void inst_lda(cpu_6502 *cpu_6502, uint16_t op_addr){
    uint8_t mem = readMemory(cpu_6502, op_addr);
    cpu_tick(cpu_6502);
    cpu_6502->regs.a = mem;
}

//load index x with memory
void inst_ldx(cpu_6502 *cpu_6502, uint16_t op_addr){
    uint8_t mem = readMemory(cpu_6502, op_addr);
    cpu_tick(cpu_6502);
    cpu_6502->regs.x = mem;
}

//load index y with memory
void inst_ldy(cpu_6502 *cpu_6502, uint16_t op_addr){
    uint8_t mem = readMemory(cpu_6502, op_addr);
    cpu_tick(cpu_6502);
    cpu_6502->regs.y = mem;
}

//logical shift right
void inst_lsr(cpu_6502 *cpu_6502, uint16_t mem_addr){
    uint8_t mem = readMemory(cpu_6502, mem_addr);
    cpu_tick(cpu_6502);
    cpu_6502->regs.p.c = (mem&1);
    mem = mem>>1;
    setZNflag(cpu_6502, mem);
    cpu_tick(cpu_6502);
    writeMemory(cpu_6502, mem_addr, mem);
    cpu_tick(cpu_6502);
}

//accumulator logical shift right
void inst_lsr_a(cpu_6502 *cpu_6502){
    cpu_6502->regs.p.c = (cpu_6502->regs.a&1);
    cpu_6502->regs.a = cpu_6502->regs.a>>1;
    setZNflag(cpu_6502, cpu_6502->regs.a);
}

//no operation, empty function
void inst_nop(){
    //la funzione di (               )
}

//or memory with accumulator
void inst_ora(cpu_6502 *cpu_6502, uint16_t op_addr){
    uint8_t operand = readMemory(cpu_6502, op_addr);
    cpu_tick(cpu_6502);
    cpu_6502->regs.a |= operand;
    setZNflag(cpu_6502, cpu_6502->regs.a);
}

//push accumulator to stack
void inst_pha(cpu_6502 *cpu_6502){
    push_s(cpu_6502, cpu_6502->regs.a); //tick 3
}

//push status register to stack
void isnt_php(cpu_6502 *cpu_6502){
    uint8_t srByte = 0;
    srByte |= cpu_6502->regs.p.c; //bit 0
    srByte |= cpu_6502->regs.p.z << 1; //bit 1
    srByte |= cpu_6502->regs.p.i << 2; //bit 2
    srByte |= cpu_6502->regs.p.b << 4; //bit 4
    srByte |= cpu_6502->regs.p.v << 6; //bit 6
    srByte |= cpu_6502->regs.p.n << 7; //bit 7
    push_s(cpu_6502, srByte); //tick 3
}

//pull accumulator from stack
void inst_pla(cpu_6502 *cpu_6502){
    cpu_6502->regs.s++;
    cpu_tick(cpu_6502); //tick 3
    cpu_6502->regs.a = pull_s(cpu_6502); 
    cpu_tick(cpu_6502);//tick 4
    setZNflag(cpu_6502->regs.a, &cpu_6502->regs);
}

//pull processor status from stack
void inst_plp(cpu_6502 *cpu_6502){
    cpu_6502->regs.s++;
    cpu_tick(cpu_6502); //tick 3
    uint8_t srByte = pull_s(cpu_6502); 
    cpu_tick(cpu_6502);//tick 4
    cpu_6502->regs.p.c = srByte&1;
    cpu_6502->regs.p.z = (srByte&2)>>1;
    cpu_6502->regs.p.i = (srByte&4)>>2;
    cpu_6502->regs.p.b = (srByte&16)>>4;
    cpu_6502->regs.p.v = (srByte&64)>>6;
    cpu_6502->regs.p.n = (srByte&128)>>7;
}

//rotate memory left
void inst_rol(cpu_6502 *cpu_6502, uint16_t mem_addr){
    uint8_t mem = readMemory(cpu_6502, mem_addr);
    cpu_tick(cpu_6502);
    cpu_6502->regs.p.c = (mem>=128);
    mem = mem<<1;
    if(cpu_6502->regs.p.c){
        mem++;
    }
    setZNflag(cpu_6502, mem);
    cpu_tick(cpu_6502);
    writeMemory(cpu_6502, mem_addr, mem);
    cpu_tick(cpu_6502);
}

//rotate accumulator left
void inst_rol_a(cpu_6502 *cpu_6502){
    cpu_6502->regs.p.c = (cpu_6502->regs.a>=128);
    cpu_6502->regs.a<<=1;
    if(cpu_6502->regs.p.c){
        cpu_6502->regs.a++;
    }
    setZNflag(cpu_6502, cpu_6502->regs.a);
}

//rotate memory right
void inst_ror(cpu_6502 *cpu_6502, uint16_t mem_addr){
    uint8_t mem = readMemory(cpu_6502, mem_addr);
    cpu_tick(cpu_6502);
    cpu_6502->regs.p.c = (mem&1);
    mem>>=1;
    if(cpu_6502->regs.p.c){
        mem+=128;
    }
    setZNflag(cpu_6502, mem);
    cpu_tick(cpu_6502);
    writeMemory(cpu_6502, mem_addr, mem);
    cpu_tick(cpu_6502);
}

//rotate accumulator right
void inst_ror_a(cpu_6502 *cpu_6502){
    cpu_6502->regs.p.c = (cpu_6502->regs.a&1);
    cpu_6502->regs.a>>=1;
    if(cpu_6502->regs.p.c){
        cpu_6502->regs.a+=128;
    }
    setZNflag(cpu_6502, cpu_6502->regs.a);
}

//return from interrupt 
void inst_rti(cpu_6502 *cpu_6502){
    //pulling P from stack
    cpu_6502->regs.s++;
    cpu_tick(cpu_6502); //3 increment s
    uint8_t srByte = pull_s(cpu_6502);
    cpu_6502->regs.p.c = srByte&1;
    cpu_6502->regs.p.z = (srByte&2)>>1;
    cpu_6502->regs.p.i = (srByte&4)>>2;
    //leave b flag ignored
    cpu_6502->regs.p.v = (srByte&64)>>6;
    cpu_6502->regs.p.n = (srByte&128)>>7;
    cpu_6502->regs.s++;
    cpu_tick(cpu_6502); //4 pull p from stack, increment s
    
    cpu_6502->regs.pc = pull_s(cpu_6502);
    cpu_6502->regs.s++;
    cpu_tick(cpu_6502); //5 pull pcl, increment s
    cpu_6502->regs.pc |= pull_s(cpu_6502)<<8;
    cpu_tick(cpu_6502); //6 pull pch
}

//return from subroutine
void inst_rts(cpu_6502 *cpu_6502){
    uint16_t pcValue;
    cpu_6502->regs.s++;
    cpu_tick(cpu_6502);// tick 3
    cpu_6502->regs.pc = pull_s(cpu_6502);
    cpu_6502->regs.s++;
    cpu_tick(cpu_6502); //tick 4 (pcl)
    cpu_6502->regs.pc |= pull_s(cpu_6502)<<8;
    cpu_tick(cpu_6502); //tick 5
    cpu_6502->regs.pc++;
    cpu_tick(cpu_6502); //tick 6
}

//subtract memory from accumulator with borrow
void inst_sbc(cpu_6502 *cpu_6502, uint16_t op_address){
    uint8_t operand = ~(readMemory(cpu_6502, op_address));
    cpu_tick(cpu_6502);
    //adc with negated operand
    unsigned int valueInt = operand + cpu_6502->regs.a + cpu_6502->regs.p.c;
    char var1 = (char)cpu_6502->regs.a;
    char var2 = (char)operand;
    char valueSigned = var1 + var2;
    //check for overflow or carry
    cpu_6502->regs.p.c = (valueInt>255);
    cpu_6502->regs.p.v = !(sign(cpu_6502->regs.a)^sign(operand))&&
    (sign(valueSigned!=sign(operand)));
    cpu_6502->regs.a = (uint8_t)valueInt; //store the result
    //set remaining flags
    setZNflag(cpu_6502, cpu_6502->regs.a);
}

//set carry flag
void inst_sec(cpu_6502 *cpu_6502){
    cpu_6502->regs.p.c = 1;
}

//set interrupt flag
void inst_sei(cpu_6502 *cpu_6502){
    cpu_6502->regs.p.i;
}

//store accumulator in memory
void inst_sta(cpu_6502 *cpu_6502, uint16_t mem_addr){
    writeMemory(cpu_6502, mem_addr, cpu_6502->regs.a);
    cpu_tick(cpu_6502);
}

//store x in memory
void inst_stx(cpu_6502 *cpu_6502, uint16_t mem_addr){
    writeMemory(cpu_6502, mem_addr, cpu_6502->regs.x);
    cpu_tick(cpu_6502);
}

//store y in memory
void inst_sty(cpu_6502 *cpu_6502, uint16_t mem_addr){
    writeMemory(cpu_6502, mem_addr, cpu_6502->regs.s);
    cpu_tick(cpu_6502);
}

//transfer accumulator to x
void inst_tax(cpu_6502 *cpu_6502){
    cpu_6502->regs.x = cpu_6502->regs.a;
    setZNflag(cpu_6502, cpu_6502->regs.x);
}

//transfer accumulator to y 
void inst_tay(cpu_6502 *cpu_6502){
    cpu_6502->regs.y = cpu_6502->regs.a;
    setZNflag(cpu_6502, cpu_6502->regs.y);
}

//tranfer stack pointer to x
void inst_tsx(cpu_6502 *cpu_6502){
    cpu_6502->regs.x = cpu_6502->regs.s;
    setZNflag(cpu_6502, cpu_6502->regs.x);
}

//tranfer x to accumulator
void inst_txa(cpu_6502 *cpu_6502){
    cpu_6502->regs.a = cpu_6502->regs.x;
    setZNflag(cpu_6502, cpu_6502->regs.a);
}

//transfer x to stack pointer
void inst_txs(cpu_6502 *cpu_6502){
    cpu_6502->regs.s = cpu_6502->regs.s;
}

//tranfer y to accumulator
void inst_tya(cpu_6502 *cpu_6502){
    cpu_6502->regs.a = cpu_6502->regs.y;
    setZNflag(cpu_6502, cpu_6502->regs.a);
}
/*                    
    COMMENTO DI SOTTO
*/