#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdlib.h>

#define MEM_SIZE      4096
#define BYTES_PER_CHAR 5
#define CHARSET_SIZE  80
#define CHARSET_START 0x050
#define CHARSET_END   0x0A0
#define PROGRAM_START 0x200

#define NUM_REGS      16
#define NUM_KEYS      16
#define CARRY_REG     0xF
#define STACK_SIZE    16
#define WIDTH         64
#define HEIGHT        32

#define HALT 1
#define DRAW 2

const static uint8_t font_charset[CHARSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

struct chip8_t {
    uint16_t opcode, I, pc;
    uint8_t  sp;
    uint8_t  delay_timer, sound_timer;
    uint8_t  flags;
    uint8_t  waiting_for_key, key_pressed;

    uint8_t  memory[MEM_SIZE];
    uint8_t  V[NUM_REGS];
    uint8_t  gfx[WIDTH * HEIGHT];
    uint16_t stack[STACK_SIZE];
    uint8_t  keys[NUM_KEYS];

} chip8_t;
typedef struct chip8_t chip8;

chip8*   chip8_init();
void     chip8_error(chip8* c, char* format, ...);
uint8_t  chip8_program_load(chip8* c, char* filename);
void     chip8_debug_print(chip8* c);
void     chip8_emulate_cycle(chip8* c);
void     chip8_opcode_fetch(chip8* c);
void     chip8_opcode_exec(chip8* c);
uint8_t  chip8_wait_for_key(chip8* c);
void     chip8_mem_dump(chip8* c);

static inline uint8_t  chip8_check_flag(chip8* c, uint8_t flag) { return c->flags & flag; }

static inline void chip8_mem_write8(chip8* c, uint16_t addr, uint8_t val) { c->memory[addr] = val; }
static inline uint8_t chip8_mem_read8(chip8* c, uint16_t addr) { return c->memory[addr]; }
static inline void chip8_mem_write16(chip8* c, uint16_t addr, uint16_t val) {
    chip8_mem_write8(c,addr, (val >> 8) & 0xFF);
    chip8_mem_write8(c,addr + 1, val & 0xFF);
}
static inline uint16_t chip8_mem_read16(chip8* c, uint16_t addr) {
    return (chip8_mem_read8(c,addr) << 8 | chip8_mem_read8(c,addr + 1));
}

static inline void     chip8_free(chip8* c) { free(c); }
static inline uint16_t chip8_char_get(chip8* c, uint8_t ch) { return CHARSET_START + ch * BYTES_PER_CHAR; }

//static inline void     chip8_opcode_fetch(chip8* c) { c->opcode = chip8_mem_read16(c, c->pc); }

static inline void     chip8_pc_set(chip8* c, uint16_t val) { c->pc = val % MEM_SIZE; }
static inline uint16_t chip8_pc_get(chip8* c) { return c->pc; }
static inline void     chip8_pc_incr(chip8* c) { chip8_pc_set(c, chip8_pc_get(c)+2); }

static inline void     chip8_index_set(chip8* c, uint16_t val) { c->I = val % MEM_SIZE; }
static inline uint16_t chip8_index_get(chip8* c) { return c->I; }

static inline void     chip8_reg_set(chip8* c, uint8_t x, uint8_t val) { c->V[x] = val; }
static inline uint8_t  chip8_reg_get(chip8* c, uint8_t x) { return c->V[x]; }

static inline void     chip8_stack_push(chip8* c) { c->stack[c->sp++] = chip8_pc_get(c); }
static inline void     chip8_stack_pop(chip8* c) { chip8_pc_set(c, c->stack[--c->sp]); }

static inline void     chip8_key_set(chip8* c, uint8_t key, uint8_t val) { c->keys[key & 0xF] = val; }
static inline uint8_t  chip8_key_get(chip8* c, uint8_t key) { return c->keys[key & 0xF]; }


#endif
