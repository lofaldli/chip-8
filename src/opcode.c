#include <stdio.h>
#include <stdlib.h>
#include "chip8.h"

#define X ((c->opcode & 0x0F00) >> 8)
#define Y ((c->opcode & 0x00F0) >> 4)
#define N (c->opcode & 0x000F)
#define KK (c->opcode & 0x00FF)
#define ADDR (c->opcode & 0x0FFF)

typedef void (*chip8_func_ptr)(chip8*);

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
}

/*
 * stores registers V0-Vx in memory starting at I
 * */
void chip8_op_store(chip8* c, uint8_t x) {
    uint16_t index = chip8_index_get(c);
    for (uint8_t i=0; i<=x; i++)
        chip8_mem_write8(c, index+i, chip8_reg_get(c,i));
}

/*
 * load registers V0-Vx from memory starting at I
 * */
void chip8_op_load(chip8* c, uint8_t x) {
    uint16_t index = chip8_index_get(c);
    for (uint8_t i=0; i<=x; i++)
        chip8_reg_set(c,i, chip8_mem_read8(c, index+i));
}

void chip8_op_0xxx(chip8* c) {

    if (KK == 0x00) {
        /* do nothing */
        chip8_pc_incr(c);

    } else if (KK == 0xE0) {
        /* clear screen */
        for (uint16_t i=0; i<WIDTH*HEIGHT; i++)
            c->gfx[i] = 0;
        chip8_pc_incr(c);

    } else if (KK == 0xEE) {
        /* return from subroutine or halt program */
        if (c->sp > 0) {
            chip8_stack_pop(c);
        } else {
            chip8_error(c, "halting program; restart required\n");
        }

    } else {
        chip8_error(c, "invalid opcode [0x0000]: 0x%04X", c->opcode);
    }
}

void chip8_op_1xxx(chip8* c) {
    /* JMP ADDR */
    chip8_pc_set(c, ADDR);
}

void chip8_op_2xxx(chip8* c) {
    /* CALL ADDR */
    if (c->sp == STACK_SIZE) {
        chip8_error(c, "stack overflow!");
        return;
    }
    chip8_stack_push(c);
    chip8_pc_set(c,ADDR);
}

void chip8_op_3xxx(chip8* c) {
    /* skip next instruction if Vx == kk */
    if (c,chip8_reg_get(c,X) == KK)
        chip8_pc_incr(c);
    chip8_pc_incr(c);
}

void chip8_op_4xxx(chip8* c) {
    /* skip next instruction if Vx != kk */
    if (c,chip8_reg_get(c,X) != KK)
        chip8_pc_incr(c);
    chip8_pc_incr(c);
}

void chip8_op_5xxx(chip8* c) {
    /* skip next instruction if Vx == Vy */
    if (c,chip8_reg_get(c,X) == chip8_reg_get(c,Y))
        chip8_pc_incr(c);
    chip8_pc_incr(c);
}

void chip8_op_6xxx(chip8* c) {
    /* Vx = kk */
    chip8_reg_set(c,X,KK);
    chip8_pc_incr(c);
}

void chip8_op_7xxx(chip8* c) {
    /* Vx = Vx + kk */
    if (KK > 0xFF - chip8_reg_get(c,X))
        chip8_reg_set(c, CARRY_REG, 1);
    else
        chip8_reg_set(c, CARRY_REG, 0);
    chip8_reg_set(c,X, chip8_reg_get(c,X) + KK);
    chip8_pc_incr(c);
}

void chip8_op_8xxx(chip8* c) {
    switch (N) {
        case 0x0000:
            chip8_reg_set(c,X, chip8_reg_get(c,Y));
            break;
        case 0x0001:
            chip8_reg_set(c,X, chip8_reg_get(c,X) & chip8_reg_get(c,Y));
            break;
        case 0x0002:
            chip8_reg_set(c,X, chip8_reg_get(c,X) | chip8_reg_get(c,Y));
            break;
        case 0x0003:
            chip8_reg_set(c,X, chip8_reg_get(c,X) ^ chip8_reg_get(c,Y));
            break;
        case 0x0004:
            chip8_reg_set(c,CARRY_REG,
                    (chip8_reg_get(c,Y) > 0xFF - chip8_reg_get(c,X)) ? 1:0);
            chip8_reg_set(c,X, chip8_reg_get(c,X) + chip8_reg_get(c,Y));
            break;
        case 0x0005:
            chip8_reg_set(c,CARRY_REG,
                    (chip8_reg_get(c,Y) > chip8_reg_get(c,X)) ? 1:0);
            chip8_reg_set(c,X, chip8_reg_get(c,X) - chip8_reg_get(c,Y));
            break;
        case 0x0006:
            chip8_reg_set(c,CARRY_REG, (chip8_reg_get(c,X) & 0x01) ? 1:0);
            chip8_reg_set(c,X, chip8_reg_get(c,X) >> 1);
            break;
        case 0x0007:
            chip8_reg_set(c,CARRY_REG,
                    (chip8_reg_get(c,X) > chip8_reg_get(c,Y)) ? 1:0);
            chip8_reg_set(c,X, chip8_reg_get(c,Y) - chip8_reg_get(c,X));
            break;
        case 0x000E:
            chip8_reg_set(c,CARRY_REG, (chip8_reg_get(c,X) & 0x80) ? 1:0);
            chip8_reg_set(c,X, chip8_reg_get(c,X) << 1);
            break;
        default: chip8_error(c, "invalid opcode [0x8000]: 0x%04X", c->opcode); break;
    }
    chip8_pc_incr(c);
}

void chip8_op_9xxx(chip8* c) {
    if (chip8_reg_get(c,X) != chip8_reg_get(c,Y))
        chip8_pc_incr(c);
    chip8_pc_incr(c);
}

void chip8_op_axxx(chip8* c) {
    chip8_index_set(c, ADDR);
    chip8_pc_incr(c);
}

void chip8_op_bxxx(chip8* c) {
    chip8_pc_set(c, ADDR + chip8_reg_get(c,0));
}

void chip8_op_cxxx(chip8* c) {
    /* Vx = rand(0,255) & kk */
    chip8_reg_set(c,X, (rand() % 0xFF) & KK);
    chip8_pc_incr(c);
}

void chip8_op_dxxx(chip8* c) {
    /*
     * draws sprite at (x,y) of size n from
     * memory location I
     * VF = 1 if collision
     * */
    uint8_t pixel;
    uint8_t Vx = chip8_reg_get(c,X);
    uint8_t Vy = chip8_reg_get(c,Y);
    uint16_t index = chip8_index_get(c);

    chip8_reg_set(c,CARRY_REG, 0);
    for (uint8_t yline=0; yline<N; yline++) {

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
    chip8_pc_incr(c);
}

void chip8_op_exxx(chip8* c) {
    if (KK == 0x009E) {

        if (c, chip8_key_get(c,chip8_reg_get(c,X)) != 0)
            chip8_pc_incr(c);
        chip8_pc_incr(c);

    } else if (KK == 0x00A1) {

        if (chip8_key_get(c,chip8_reg_get(c,X)) == 0)
            chip8_pc_incr(c);
        chip8_pc_incr(c);

    } else {
        chip8_error(c, "invalid opcode [0xE000]: 0x%04X", c->opcode);
    }
}

void chip8_op_fxxx(chip8* c) {
    switch (KK) {
        case 0x0007: chip8_reg_set(c,X, c->delay_timer); break;
        case 0x000A: chip8_reg_set(c,X, chip8_wait_for_key(c)); break;
        case 0x0015: c->delay_timer = chip8_reg_get(c,X); break;
        case 0x0018: c->sound_timer = chip8_reg_get(c,X); break;
        case 0x001E: chip8_index_set(c, c->I + chip8_reg_get(c,X)); break;
        case 0x0029: chip8_index_set(c, chip8_char_get(c, chip8_reg_get(c,X))); break;
        case 0x0033: chip8_op_bcd(c,X); break;
        case 0x0055: chip8_op_store(c,X); break;
        case 0x0065: chip8_op_load(c,X); break;
        default: chip8_error(c, "invalid opcode [0xF000]: 0x%04X", c->opcode); break;
    }
    chip8_pc_incr(c);
}



chip8_func_ptr func_table[16] = {
    chip8_op_0xxx, chip8_op_1xxx, chip8_op_2xxx, chip8_op_3xxx,
    chip8_op_4xxx, chip8_op_5xxx, chip8_op_6xxx, chip8_op_7xxx,
    chip8_op_8xxx, chip8_op_9xxx, chip8_op_axxx, chip8_op_bxxx,
    chip8_op_cxxx, chip8_op_dxxx, chip8_op_exxx, chip8_op_fxxx
};

/*
 * sets opcode as specified by pc
 * */
void chip8_opcode_fetch(chip8* c) {
    c->opcode = chip8_mem_read16(c, c->pc);
}


/*
 * executes the current opcode
 * */
void chip8_opcode_exec(chip8* c) {

    chip8_func_ptr func = func_table[(c->opcode & 0xF000) >> 12];

    func(c);

}
