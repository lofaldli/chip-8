#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"


/*
 * sets opcode as specified by pc
 * */
void chip8_opcode_fetch(chip8* c) {
    c->opcode = chip8_mem_read16(c, c->pc);
}

/*
 * clear gfx buffer (screen)
 * */
void chip8_op_cls(chip8* c) {
    for (uint16_t i=0; i<WIDTH*HEIGHT; i++)
        c->gfx[i] = 0;
    chip8_pc_incr(c);
}

/*
 * return from subroutine e.g:
 *   sp--
 *   pc=stack[sp]
 * if sp=0:
 *   halt program (useful for stopping without exiting)
 * */
void chip8_op_return(chip8* c) {
    if (c->sp > 0) {
        chip8_stack_pop(c);
    } else {
        chip8_error(c, "halting program; restart required\n");
    }
}

/*
 * jump to instruction (pc=addr)
 * */
void chip8_op_jump(chip8* c, uint16_t addr) {
    chip8_pc_set(c,addr);
}

/*
 * call subroutine at addr:
 *   stack[sp] = pc
 *   sp++
 *   pc = addr
 * */
void chip8_op_call(chip8* c, uint16_t addr) {
    if (c->sp == STACK_SIZE) {
        chip8_error(c, "stack overflow!");
        return;
    }
    chip8_stack_push(c);
    chip8_pc_set(c,addr);
}

/*
 * skip next instruction if cond is true
 * */
void chip8_op_skip(chip8* c, uint8_t cond) {
    if (cond)
        chip8_pc_incr(c);
    chip8_pc_incr(c);
}

void chip8_op_ld_reg(chip8* c, uint8_t x, uint8_t val) {
    chip8_reg_set(c,x,val);
    chip8_pc_incr(c);
}

void chip8_op_ld_index(chip8* c, uint16_t val) {
    chip8_index_set(c,val);
    chip8_pc_incr(c);
}

void chip8_op_ld_dt(chip8* c, uint8_t val) {
    c->delay_timer = val;
    chip8_pc_incr(c);
}

void chip8_op_ld_st(chip8* c, uint8_t val) {
    c->sound_timer = val;
    chip8_pc_incr(c);
}

/*
 * Vx = Vx + kk
 * VF = 1 if overflow
 * */
void chip8_op_addi(chip8* c, uint8_t x, uint8_t kk) {
    if (kk > 0xFF - chip8_reg_get(c,x))
        chip8_reg_set(c, CARRY_REG, 1);
    else
        chip8_reg_set(c, CARRY_REG, 0);
    chip8_reg_set(c,x, chip8_reg_get(c,x) + kk);
    chip8_pc_incr(c);
}

/*
 * Vx = Vx + Vy
 * VF = 1 if overflow
 * */
void chip8_op_add(chip8* c, uint8_t x, uint8_t y) {
    if (chip8_reg_get(c,y) > 0xFF - chip8_reg_get(c,x))
        chip8_reg_set(c, CARRY_REG, 1);
    else
        chip8_reg_set(c, CARRY_REG, 0);
    chip8_reg_set(c,x, chip8_reg_get(c,x) + chip8_reg_get(c,y));
    chip8_pc_incr(c);
}

/*
 * Vx = Vx & Vy
 * */
void chip8_op_and(chip8* c, uint8_t x, uint8_t y) {
    chip8_reg_set(c,x, chip8_reg_get(c,x) & chip8_reg_get(c,y));
    chip8_pc_incr(c);
}

/*
 * Vx = Vx | Vy
 * */
void chip8_op_or(chip8* c, uint8_t x, uint8_t y) {
    chip8_reg_set(c,x, chip8_reg_get(c,x) | chip8_reg_get(c,y));
    chip8_pc_incr(c);
}

/*
 * Vx = Vx ^ Vy
 * */
void chip8_op_xor(chip8* c, uint8_t x, uint8_t y) {
    chip8_reg_set(c,x, chip8_reg_get(c,x) ^ chip8_reg_get(c,y));
    chip8_pc_incr(c);
}

/*
 * Vx = Vx - Vy
 * VF = 1 if Vy > Vx
 * */
void chip8_op_sub(chip8* c, uint8_t x, uint8_t y) {
    if (chip8_reg_get(c,y) > chip8_reg_get(c,x))
        chip8_reg_set(c,CARRY_REG,1);
    else
        chip8_reg_set(c,CARRY_REG,0);
    chip8_reg_set(c, x, chip8_reg_get(c,x) - chip8_reg_get(c,y));
    chip8_pc_incr(c);
}

/*
 * Vx = Vx >> 1
 * VF = 1 if LSB = 1
 * */
void chip8_op_shr(chip8* c, uint8_t x, uint8_t y) {
    if ((chip8_reg_get(c,x) & 0x01) == 0x01)
        chip8_reg_set(c,CARRY_REG,1);
    else {
        chip8_reg_set(c,CARRY_REG,0);
        chip8_reg_set(c,x, chip8_reg_get(c,x) >> 1);
    }
    chip8_pc_incr(c);
}

/*
 * Vx = Vy - Vx
 * VF = 1 if Vx > Vy
 * */
void chip8_op_subn(chip8* c, uint8_t x, uint8_t y) {
    if (chip8_reg_get(c,x) > chip8_reg_get(c,y))
        chip8_reg_set(c,CARRY_REG,1);
    else
        chip8_reg_set(c,CARRY_REG,0);
    chip8_reg_set(c, x, chip8_reg_get(c,y) - chip8_reg_get(c,x));
    chip8_pc_incr(c);
}

/*
 * Vx = Vx << 1
 * VF = 1 if MSB = 1
 * */
void chip8_op_shl(chip8* c, uint8_t x, uint8_t y) {
    if ((chip8_reg_get(c,x) & 0x80) == 0x80)
        chip8_reg_set(c,CARRY_REG,1);
    else {
        chip8_reg_set(c,CARRY_REG,0);
        chip8_reg_set(c,x, chip8_reg_get(c,x) << 1);
    }
    chip8_pc_incr(c);
}

/*
 * Vx = rand(0,255) & kk
 * */
void chip8_op_rand(chip8* c, uint8_t x, uint8_t kk) {
    chip8_reg_set(c,x, (rand() % 0xFF) & kk);
    chip8_pc_incr(c);
}

/*
 * draws sprite at (x,y) of size n from
 * memory location I
 * VF = 1 if collision
 * */
void chip8_op_draw(chip8* c, uint8_t x, uint8_t y, uint8_t n) {
    uint8_t pixel;
    uint8_t Vx = chip8_reg_get(c,x);
    uint8_t Vy = chip8_reg_get(c,y);
    uint16_t index = chip8_index_get(c);

    chip8_reg_set(c,CARRY_REG, 0);
    for (uint8_t yline=0; yline<n; yline++) {

        pixel = c->memory[index + yline];
        for (uint8_t xline=0; xline<8; xline++) {

            if ((pixel & (0x80>>xline)) != 0) {

                if (c->gfx[(Vx + xline + ((Vy+yline)*WIDTH))] == 1)
                    chip8_reg_set(c,CARRY_REG,1);
                c->gfx[(Vx + xline + ((Vy+yline)*WIDTH))] ^= 1;
            }
        }
    }
    c->flags |= DRAW;
    /*c->draw_flag = 1;*/
    chip8_pc_incr(c);
}

/*
 * stores Binary Coded Decimal (BCD)-representation of
 * Vx at in memory I, I+1 and I+2
 * */
void chip8_op_bcd(chip8* c, uint8_t x) {
    uint8_t Vx = chip8_reg_get(c,x);
    uint16_t index = chip8_index_get(c);
    chip8_mem_write8(c, index,    Vx / 100);
    chip8_mem_write8(c, index+1, (Vx % 100) / 10);
    chip8_mem_write8(c, index+2, (Vx % 10));
    chip8_pc_incr(c);
}

/*
 * stores registers V0-Vx in memory starting at I
 * */
void chip8_op_store(chip8* c, uint8_t x) {
    uint16_t index = chip8_index_get(c);
    for (uint8_t i=0; i<=x; i++)
        chip8_mem_write8(c, index+i, chip8_reg_get(c,i));
    chip8_pc_incr(c);
}

/*
 * load registers V0-Vx from memory starting at I
 * */
void chip8_op_load(chip8* c, uint8_t x) {
    uint16_t index = chip8_index_get(c);
    for (uint8_t i=0; i<=x; i++)
        chip8_reg_set(c,i, chip8_mem_read8(c, index+i));
    chip8_pc_incr(c);
}

/*
 * executes the current opcode
 * */
void chip8_opcode_exec(chip8* c) {

    uint8_t x = (c->opcode & 0x0F00) >> 8;
    uint8_t y = (c->opcode & 0x00F0) >> 4;
    uint8_t n = c->opcode & 0x000F;
    uint8_t kk = c->opcode & 0x00FF;
    uint16_t addr = c->opcode & 0x0FFF;

    switch (c->opcode & 0xF000) {

        case 0x0000:
            switch (c->opcode) {
                case 0x0000: chip8_pc_incr(c); break; // ignore
                case 0x00E0: chip8_op_cls(c); break;
                case 0x00EE: chip8_op_return(c); break;
                default: chip8_error(c, "invalid opcode [0x0000]: 0x%04X", c->opcode);
            }
            break;

        case 0x1000: chip8_op_jump(c,addr); break;
        case 0x2000: chip8_op_call(c,addr); break;
        case 0x3000: chip8_op_skip(c,chip8_reg_get(c,x) == kk); break;
        case 0x4000: chip8_op_skip(c,chip8_reg_get(c,x) != kk); break;
        case 0x5000: chip8_op_skip(c,chip8_reg_get(c,x) == chip8_reg_get(c,y)); break;
        case 0x6000: chip8_op_ld_reg(c,x,kk); break;
        case 0x7000: chip8_op_addi(c,x,kk); break;

        case 0x8000:

            switch (n) {
                case 0x0000: chip8_op_ld_reg(c,x, chip8_reg_get(c,y)); break;
                case 0x0001: chip8_op_and(c,x,y); break;
                case 0x0002: chip8_op_or(c,x,y); break;
                case 0x0003: chip8_op_xor(c,x,y); break;
                case 0x0004: chip8_op_add(c,x,y); break;
                case 0x0005: chip8_op_sub(c,x,y); break;
                case 0x0006: chip8_op_shr(c,x,y); break;
                case 0x0007: chip8_op_subn(c,x,y); break;
                case 0x000E: chip8_op_shl(c,x,y); break;
                default: chip8_error(c, "invalid opcode [0x8000]: 0x%04X", c->opcode); break;
            }
            break;

        case 0x9000: chip8_op_skip(c,chip8_reg_get(c,x) != chip8_reg_get(c,y)); break;
        case 0xA000: chip8_op_ld_index(c, addr); break;
        case 0xB000: chip8_op_jump(c, addr + chip8_reg_get(c,0)); break;
        case 0xC000: chip8_op_rand(c,x,kk); break;
        case 0xD000: chip8_op_draw(c,x,y,n); break;

        case 0xE000:

            switch (kk) {
                case 0x009E: chip8_op_skip(c, chip8_key_get(c,chip8_reg_get(c,x)) != 0); break;
                case 0x00A1: chip8_op_skip(c, chip8_key_get(c,chip8_reg_get(c,x)) == 0); break;
                default: chip8_error(c, "invalid opcode [0xE000]: 0x%04X", c->opcode); break;
            }
            break;

        case 0xF000:

            switch (kk) {

                case 0x0007: chip8_op_ld_reg(c,x, c->delay_timer); break;
                case 0x000A: chip8_op_ld_reg(c,x, chip8_wait_for_key(c)); break;
                case 0x0015: chip8_op_ld_dt(c, chip8_reg_get(c,x)); break;
                case 0x0018: chip8_op_ld_st(c, chip8_reg_get(c,x)); break;
                case 0x001E: chip8_op_ld_index(c, c->I + chip8_reg_get(c,x)); break;
                case 0x0029: chip8_op_ld_index(c, chip8_char_get(c, chip8_reg_get(c,x))); break;
                case 0x0033: chip8_op_bcd(c,x); break;
                case 0x0055: chip8_op_store(c,x); break;
                case 0x0065: chip8_op_load(c,x); break;
                default: chip8_error(c, "invalid opcode [0xF000]: 0x%04X", c->opcode); break;

            }
            break;

        default: chip8_error(c, "invalid opcode: 0x%04X", c->opcode); break;
    }

}
