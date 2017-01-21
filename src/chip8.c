#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "chip8.h"

/*
 * sets the initial state of a chip8
 * */
chip8* chip8_init() {
    chip8* c = malloc(sizeof(chip8));

    c->opcode = 0;
    c->I = 0;
    c->pc = PROGRAM_START;
    c->delay_timer = 0;
    c->sound_timer = 0;
    c->sp = 0;
    c->flags = 0;
    c->waiting_for_key = 0;
    c->key_pressed = -1;

    uint16_t i;
    for (i=0; i<NUM_REGS; i++)     chip8_reg_set(c, i, 0);
    for (i=0; i<WIDTH*HEIGHT; i++) c->gfx[i] = 0;
    for (i=0; i<STACK_SIZE; i++)   c->stack[i] = 0;
    for (i=0; i<NUM_KEYS; i++)     c->keys[i] = 0;
    for (i=0; i<MEM_SIZE; i++)     chip8_mem_write8(c,i,0);
    for (i=0; i<CHARSET_SIZE; i++) chip8_mem_write8(c,CHARSET_START + i, font_charset[i]);

    return c;
}

/*
 * print an error message and set halt flag
 * */
void chip8_error(chip8* c, char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 256, format, args);
    va_end(args);
    perror(buffer);

    c->flags |= HALT;
}

/*
 * load a program from a file
 * */
uint8_t chip8_program_load(chip8* c, char* filename) {

    // open file in read+binary mode
    FILE* f = fopen(filename, "rb");
    if (f == NULL) {
        fprintf(stderr, "could not find \"%s\"\n", filename);
        return 1;
    }

    // get file size
    fseek(f, 0, SEEK_END);
    uint16_t size = ftell(f);
    rewind(f);

    if (size > MEM_SIZE - PROGRAM_START) {
        fprintf(stderr, "Not enough memory: %i", size);
        return 1;
    }

    // allocate buffer
    uint8_t* buffer = malloc(sizeof(uint8_t) * size);

    // copy file into buffer
    fread(buffer, 1, size, f);

    for (uint16_t i=0; i<size; i++) {
        c->memory[i+PROGRAM_START] = buffer[i];
    }
    return 0;
}

/*
 * will only increment pc if key press is registered
 * */
uint8_t chip8_wait_for_key(chip8* c) {
    // TODO
    chip8_pc_incr(c);
    return 0;
}

void chip8_update_timers(chip8* c) {
    if (c->delay_timer > 0)
        c->delay_timer--;

    if (c->sound_timer > 0) {
        if (c->sound_timer == 1)
            printf("BEEP\n");
        c->sound_timer--;
    }
}

void chip8_debug_print(chip8* c) {
    fprintf(stderr, "op: 0x%04X pc: 0x%03X sp: 0x%02X I: 0x%03X\n", c->opcode, c->pc, c->sp, c->I);

    fprintf(stderr, "registers ");
    for (uint8_t i=0; i<NUM_REGS; i++) {
        fprintf(stderr, "0x%02X ",chip8_reg_get(c,i));
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "stack     ");
    if (c->sp == 0) {
        fprintf(stderr, "empty");
    } else {
        for (uint8_t i=0; i<STACK_SIZE && i<c->sp; i++) {
            fprintf(stderr, "0x%03X ", c->stack[i]);
        }
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "keys      ");
    for (uint8_t i=0; i<NUM_KEYS; i++) {
        if (c->keys[i] != 0) {
            fprintf(stderr, "%X ", i);
        }
    }
    fprintf(stderr, "\n\n");
}

/*
 * dump content of chip8 memory to a file
 * */
void chip8_mem_dump(chip8* c) {
    FILE* f = fopen("memory.dump", "wb");
    fwrite(c->memory, MEM_SIZE, 1, f);
    fclose(f);
}


void chip8_emulate_cycle(chip8* c) {
    chip8_opcode_fetch(c);
    chip8_opcode_exec(c);
    chip8_update_timers(c);
}
