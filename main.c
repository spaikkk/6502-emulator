#include <stdio.h>
#include "functions.h"

int main(int argc, char **argv) {

    //initialize the cpu
    cpu_6502 cpu;
    cpu_init(&cpu);

    //read the rom and store it in cpu struct
    FILE* file;
    file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("Cribbio!\n");
        return 1;
    }
    fseek(file, 0, SEEK_END);
    int dims = ftell(file);
    fseek(file, 0, SEEK_SET);
    size_t num = fread(cpu.mem.rom, 1, dims, file);
    fclose(file);

    //cycle of execution
    unsigned long cycle = 0;
    for(;;){
        uint8_t opCode = readMemory(&cpu, cpu.regs.pc);
        uint16_t addr;
        cpu.regs.pc++;
        cpu_tick(&cpu);
        switch (opCode) {
            case 0x0:
                implied(&cpu);
                inst_brk(&cpu);
                break;
            case 0x1:
                addr = preIndexed_indirect(&cpu);
                inst_ora(&cpu, addr);
                break;
            case 0x5:
                addr = zeropage(&cpu, 0);
                inst_ora(&cpu, addr);
                break;
            case 0x6:
                addr = zeropage(&cpu, 0);
                inst_asl(&cpu, addr);
                break;
            case 0x8:
                implied(&cpu);
                isnt_php(&cpu);
                break;
            case 0x9:
                addr = immediate(&cpu);
                inst_ora(&cpu, addr);
                break;
            case 0xa:
                //ACCUMULATOR
                inst_asl_ac(&cpu);
                break;
            case 0xd:
                addr = absolute(&cpu);
                addr = absolute(&cpu);
                inst_ora(&cpu, addr);
                break;
            case 0xe:
                addr = absolute(&cpu);
                inst_asl(&cpu, addr);
                break;
            case 0x10:
                signed char offset = relative(&cpu);
                inst_bpl(&cpu, offset);
                break;
            case 0x11:
                addr = postIndexed_indirect(&cpu);
                inst_ora(&cpu, addr);
                break;
            case 0x15:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_ora(&cpu, addr);
                break;
            case 0x16:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_asl(&cpu, addr);
                break;
            case 0x18:
                implied(&cpu);
                inst_clc(&cpu);
                break;
            case 0x19:
                addr = absolute_indexed(&cpu, cpu.regs.y);
                inst_ora(&cpu, addr);
                break;
            case 0x1d:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_ora(&cpu, addr);
                break;
            case 0x1e:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_asl(&cpu, addr);
                break;
            case 0x20:
                addr = absolute(&cpu);
                inst_jsr(&cpu);
                break;
            case 0x21:
                addr = preIndexed_indirect(&cpu);
                inst_and(&cpu, addr);
                break;
            case 0x24:
                addr = zeropage(&cpu, 0);
                inst_bit(&cpu, addr);
                break;
            case 0x25:
                addr = zeropage(&cpu, 0);
                inst_and(&cpu, addr);
                break;
            case 0x26:
                addr = zeropage(&cpu, 0);
                inst_rol(&cpu, addr);
                ROL();
                break;
            case 0x28:
                implied(&cpu);
                inst_plp(&cpu);
                break;
            case 0x29:
                addr = immediate(&cpu);
                inst_and(&cpu, addr);
                break;
            case 0x2a:
                //ACCUMULATOR
                inst_rol_a(&cpu, addr);
                break;
            case 0x2c:
                addr = absolute(&cpu);
                inst_bit(&cpu, addr);
                break;
            case 0x2d:
                addr = absolute(&cpu);
                inst_and(&cpu, addr);
                break;
            case 0x2e:
                addr = absolute(&cpu);
                inst_rol(&cpu, addr);
                break;
            case 0x30:
                signed char offset = relative(&cpu);
                inst_bmi(&cpu, offset);
                break;
            case 0x31:
                addr = postIndexed_indirect(&cpu);
                inst_and(&cpu, addr);
                break;
            case 0x35:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_and(&cpu, addr);
                break;
            case 0x36:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_rol(&cpu, addr);
                break;
            case 0x38:
                implied(&cpu);
                inst_sec(&cpu);
                break;
            case 0x39:
                addr = absolute_indexed(&cpu, cpu.regs.y);
                inst_and(&cpu, addr);
                break;
            case 0x3d:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_and(&cpu, addr);
                break;
            case 0x3e:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_rol(&cpu, addr);
                break;
            case 0x40:
                implied(&cpu);
                inst_rti(&cpu);
                break;
            case 0x41:
                addr = preIndexed_indirect(&cpu);
                inst_eor(&cpu, addr);
                break;
            case 0x45:
                addr = zeropage(&cpu, 0);
                inst_eor(&cpu, addr);
                break;
            case 0x46:
                addr = zeropage(&cpu, 0);
                inst_lsr(&cpu, addr);
                break;
            case 0x48:
                implied(&cpu);
                inst_pha(&cpu);
                break;
            case 0x49:
                addr = immediate(&cpu);
                inst_eor(&cpu, addr);
                break;
            case 0x4a:
                //ACCUMULATOR
                inst_lsr_a(&cpu, addr);
                break;
            case 0x4c:
                addr = absolute(&cpu);
                inst_abs_jmp(&cpu, addr);
                break;
            case 0x4d:
                addr = absolute(&cpu);
                inst_eor(&cpu, addr);
                break;
            case 0x4e:
                addr = absolute(&cpu);
                inst_lsr(&cpu, addr);
                break;
            case 0x50:
                signed char offset = relative(&cpu);
                inst_bvc(&cpu, offset);
                break;
            case 0x51:
                addr = postIndexed_indirect(&cpu);
                inst_eor(&cpu, addr);
                break;
            case 0x55:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_eor(&cpu, addr);
                break;
            case 0x56:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_lsr(&cpu, addr);
                break;
            case 0x58:
                implied(&cpu);
                inst_cli(&cpu);
                break;
            case 0x59:
                addr = absolute_indexed(&cpu, cpu.regs.y);
                inst_eor(&cpu, addr);
                break;
            case 0x5d:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_eor(&cpu, addr);
                break;
            case 0x5e:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_lsr(&cpu, addr);
                break;
            case 0x60:
                implied(&cpu);
                inst_rts(&cpu);
                break;
            case 0x61:
                addr = preIndexed_indirect(&cpu);
                inst_adc(&cpu, addr);
                break;
            case 0x65:
                addr = zeropage(&cpu, 0);
                inst_adc(&cpu, addr);
                break;
            case 0x66:
                addr = zeropage(&cpu, 0);
                inst_ror(&cpu, addr);
                break;
            case 0x68:
                implied(&cpu);
                inst_pla(&cpu);
                break;
            case 0x69:
                addr = immediate(&cpu);
                inst_adc(&cpu, addr);
                break;
            case 0x6a:
                //ACCUMULATOR
                inst_ror_a(&cpu, addr);
                break;
            case 0x6c:
                inst_ind_jmp(&cpu);
                break;
            case 0x6d:
                addr = absolute(&cpu);
                inst_adc(&cpu, addr);
                break;
            case 0x6e:
                addr = absolute(&cpu);
                inst_ror(&cpu, addr);
                break;
            case 0x70:
                signed char offset = relative(&cpu);
                inst_bvs(&cpu, offset);
                break;
            case 0x71:
                addr = postIndexed_indirect(&cpu);
                inst_adc(&cpu, addr);
                break;
            case 0x75:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_adc(&cpu, addr);
                break;
            case 0x76:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_ror(&cpu, addr);
                break;
            case 0x78:
                implied(&cpu);
                inst_sei(&cpu);
                break;
            case 0x79:
                addr = absolute_indexed(&cpu, cpu.regs.y);
                inst_adc(&cpu, addr);
                break;
            case 0x7d:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_adc(&cpu, addr);
                break;
            case 0x7e:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_ror(&cpu, addr);
                break;
            case 0x81:
                addr = preIndexed_indirect(&cpu);
                inst_sta(&cpu, addr);
                break;
            case 0x84:
                addr = zeropage(&cpu, 0);
                inst_sty(&cpu, addr);
                break;
            case 0x85:
                addr = zeropage(&cpu, 0);
                inst_sta(&cpu, addr);
                break;
            case 0x86:
                addr = zeropage(&cpu, 0);
                inst_stx(&cpu, addr);
                break;
            case 0x88:
                implied(&cpu);
                inst_dey(&cpu);
                break;
            case 0x8a:
                implied(&cpu);
                inst_txa(&cpu);
                break;
            case 0x8c:
                addr = absolute(&cpu);
                inst_sty(&cpu, addr);
                break;
            case 0x8d:
                addr = absolute(&cpu);
                inst_sta(&cpu, addr);
                break;
            case 0x8e:
                addr = absolute(&cpu);
                inst_stx(&cpu, addr);
                break;
            case 0x90:
                signed char offset = relative(&cpu);
                isnt_bcc(&cpu, offset);
                break;
            case 0x91:
                addr = postIndexed_indirect(&cpu);
                inst_sta(&cpu, addr);
                break;
            case 0x94:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_sty(&cpu, addr);
                break;
            case 0x95:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_sta(&cpu, addr);
                break;
            case 0x96:
                addr = zeroPage(&cpu, cpu.regs.y);
                inst_stx(&cpu, addr);
                break;
            case 0x98:
                implied(&cpu);
                inst_tya(&cpu);
                break;
            case 0x99:
                addr = absolute_indexed(&cpu, cpu.regs.y);
                inst_sta(&cpu, addr);
                break;
            case 0x9a:
                implied(&cpu);
                inst_txs(&cpu);
                break;
            case 0x9d:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_sta(&cpu, addr);
                break;
            case 0xa0:
                addr = immediate(&cpu);
                inst_ldy(&cpu, addr);
                break;
            case 0xa1:
                addr = preIndexed_indirect(&cpu);
                inst_lda(&cpu, addr);
                break;
            case 0xa2:
                addr = immediate(&cpu);
                inst_ldx(&cpu, addr);
                break;
            case 0xa4:
                addr = zeropage(&cpu, 0);
                inst_ldy(&cpu, addr);
                break;
            case 0xa5:
                addr = zeropage(&cpu, 0);
                inst_lda(&cpu, addr);
                break;
            case 0xa6:
                addr = zeropage(&cpu, 0);
                inst_ldx(&cpu, addr);
                break;
            case 0xa8:
                implied(&cpu);
                inst_tay(&cpu);
                break;
            case 0xa9:
                addr = immediate(&cpu);
                inst_lda(&cpu, addr);
                break;
            case 0xaa:
                implied(&cpu);
                inst_tax(&cpu);
                break;
            case 0xac:
                addr = absolute(&cpu);
                inst_ldy(&cpu, addr);
                break;
            case 0xad:
                addr = absolute(&cpu);
                inst_lda(&cpu, addr);
                break;
            case 0xae:
                addr = absolute(&cpu);
                inst_ldx(&cpu, addr);
                break;
            case 0xb0:
                signed char offset = relative(&cpu);
                inst_bcs(&cpu, offset);
                break;
            case 0xb1:
                addr = postIndexed_indirect(&cpu);
                inst_lda(&cpu, addr);
                break;
            case 0xb4:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_ldy(&cpu, addr);
                break;
            case 0xb5:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_lda(&cpu, addr);
                break;
            case 0xb6:
                addr = zeroPage(&cpu, cpu.regs.y);
                inst_ldx(&cpu, addr);
                break;
            case 0xb8:
                implied(&cpu);
                inst_clv(&cpu);
                break;
            case 0xb9:
                addr = absolute_indexed(&cpu, cpu.regs.y);
                inst_lda(&cpu, addr);
                break;
            case 0xba:
                implied(&cpu);
                inst_tsx(&cpu);
                break;
            case 0xbc:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_ldy(&cpu, addr);
                break;
            case 0xbd:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_lda(&cpu, addr);
                break;
            case 0xbe:
                addr = absolute_indexed(&cpu, cpu.regs.y);
                inst_ldx(&cpu, addr);
                break;
            case 0xc0:
                addr = immediate(&cpu);
                inst_cpy(&cpu, addr);
                break;
            case 0xc1:
                addr = preIndexed_indirect(&cpu);
                inst_cmp(&cpu, addr);
                break;
            case 0xc4:
                addr = zeropage(&cpu, 0);
                inst_cpy(&cpu, addr);
                break;
            case 0xc5:
                addr = zeropage(&cpu, 0);
                inst_cmp(&cpu, addr);
                break;
            case 0xc6:
                addr = zeropage(&cpu, 0);
                inst_dec(&cpu, addr);
                break;
            case 0xc8:
                implied(&cpu);
                inst_iny(&cpu);
                break;
            case 0xc9:
                addr = immediate(&cpu);
                inst_cmp(&cpu, addr);
                break;
            case 0xca:
                implied(&cpu);
                inst_dex(&cpu);
                break;
            case 0xcc:
                addr = absolute(&cpu);
                inst_cpy(&cpu, addr);
                break;
            case 0xcd:
                addr = absolute(&cpu);
                inst_cmp(&cpu, addr);
                break;
            case 0xce:
                addr = absolute(&cpu);
                inst_dec(&cpu, addr);
                break;
            case 0xd0:
                signed char offset = relative(&cpu);
                inst_bne(&cpu, offset);
                break;
            case 0xd1:
                addr = postIndexed_indirect(&cpu);
                inst_cmp(&cpu, addr);
                break;
            case 0xd5:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_cmp(&cpu, addr);
                break;
            case 0xd6:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_dec(&cpu, addr);
                break;
            case 0xd8:
                implied(&cpu);
                //CLD();
                break;
            case 0xd9:
                addr = absolute_indexed(&cpu, cpu.regs.y);
                inst_cmp(&cpu, addr);
                break;
            case 0xdd:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_cmp(&cpu, addr);
                break;
            case 0xde:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_dec(&cpu, addr);
                break;
            case 0xe0:
                addr = immediate(&cpu);
                inst_cpx(&cpu, addr);
                break;
            case 0xe1:
                addr = preIndexed_indirect(&cpu);
                inst_sbc(&cpu, addr);
                break;
            case 0xe4:
                addr = zeropage(&cpu, 0);
                inst_cpx(&cpu, addr);
                break;
            case 0xe5:
                addr = zeropage(&cpu, 0);
                inst_sbc(&cpu, addr);
                break;
            case 0xe6:
                addr = zeropage(&cpu, 0);
                inst_inc(&cpu, addr);
                break;
            case 0xe8:
                implied(&cpu);
                inst_inx(&cpu);
                break;
            case 0xe9:
                addr = immediate(&cpu);
                inst_sbc(&cpu, addr);
                break;
            case 0xea:
                implied(&cpu);
                inst_nop();
                break;
            case 0xec:
                addr = absolute(&cpu);
                inst_cpx(&cpu, addr);
                break;
            case 0xed:
                addr = absolute(&cpu);
                inst_sbc(&cpu, addr);
                break;
            case 0xee:
                addr = absolute(&cpu);
                inst_inc(&cpu, addr);
                break;
            case 0xf0:
                signed char offset = relative(&cpu);
                inst_beq(&cpu, offset);
                break;
            case 0xf1:
                addr = postIndexed_indirect(&cpu);
                inst_sbc(&cpu, addr);
                break;
            case 0xf5:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_sbc(&cpu, addr);
                break;
            case 0xf6:
                addr = zeroPage(&cpu, cpu.regs.x);
                inst_inc(&cpu, addr);
                break;
            case 0xf8:
                implied(&cpu);
                 //SED();
                break;
            case 0xf9:
                addr = absolute_indexed(&cpu, cpu.regs.y);
                inst_sbc(&cpu, addr);
                break;
            case 0xfd:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_sbc(&cpu, addr);
                break;
            case 0xfe:
                addr = absolute_indexed(&cpu, cpu.regs.x);
                inst_inc(&cpu, addr);
                break;
        }
        cycle++;
    }

	return 0;
}