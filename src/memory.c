#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdio.h>
#include "chip8.h"

/*
 * initializes memory between 0x050 and 0x0a0
 * to charset, rest is set to zero
 * */
void chip8_mem_init(chip8* c) {
    uint16_t i;

    for (i=0; i<MEM_SIZE; i++)
        chip8_mem_write8(c,i,0);

    for (i=0; i<CHARSET_SIZE; i++)
        chip8_mem_write8(c,CHARSET_START + i, font_charset[i]);
}

void chip8_mem_dump(chip8* c) {
    FILE* f = fopen("memory.dump", "wb");
    fwrite(c->memory, MEM_SIZE, 1, f);
    fclose(f);
}

void chip8_mem_write8(chip8* c, uint16_t addr, uint8_t val) {
    c->memory[addr] = val;
}

uint8_t chip8_mem_read8(chip8* c, uint16_t addr) {
    return c->memory[addr];
}
void chip8_mem_write16(chip8* c, uint16_t addr, uint16_t val) {
    c->memory[addr + 0] = (val >> 8) & 0xFF;
    c->memory[addr + 1] = val & 0xFF;
}

uint16_t chip8_mem_read16(chip8* c, uint16_t addr) {
    return c->memory[addr] << 8 | c->memory[addr+1];
}

#endif
