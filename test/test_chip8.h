#ifndef TEST_CHIP8_H
#define TEST_CHIP8_H

#include <stdlib.h>
#include <stdint.h>
#include <check.h>
#include "../src/chip8.h"

/* some delicious macros */
#define ASSERT_PC(val) ck_assert_uint_eq((uint16_t)chip8_pc_get(c), (uint16_t)(val));
#define ASSERT_REG(x,val) ck_assert_uint_eq((uint8_t)chip8_reg_get(c,x), (uint8_t)(val));
#define EXEC(op) c->opcode = (op); chip8_opcode_exec(c);
#define DEBUG_PRINT() chip8_debug_print(c);


Suite* chip8_suite(void);
Suite* opcode_suite(void);

#endif
