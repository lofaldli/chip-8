#include "test_chip8.h"

static chip8* c;
static void setup() {
    c = chip8_init();
}
static void teardown() {
    chip8_free(c);
}

/* checks the state of a fresh chip8 */
START_TEST(test_chip8_init) {

    ck_assert_uint_eq( c->opcode, 0 );
    ck_assert_uint_eq( c->I, 0 );
    ck_assert_uint_eq( c->pc, PROGRAM_START );
    ck_assert_uint_eq( c->delay_timer, 0 );
    ck_assert_uint_eq( c->sound_timer, 0 );
    ck_assert_uint_eq( c->sp, 0 );
    ck_assert_uint_eq( c->flags, 0 );

    uint16_t i;
    for (i=0; i<NUM_REGS; i++)
        ck_assert_uint_eq(c->V[i], 0);
    for (i=0; i<WIDTH*HEIGHT; i++)
        ck_assert_uint_eq(c->gfx[i], 0);
    for (i=0; i<STACK_SIZE; i++)
        ck_assert_uint_eq(c->stack[i], 0);
    for (i=0; i<MEM_SIZE; i++)
        if (i < CHARSET_START || i >= CHARSET_END)
            ck_assert_uint_eq(c->memory[i], 0);
        else
            ck_assert_uint_eq(c->memory[i], font_charset[i-CHARSET_START]);


} END_TEST

/* checks program counter functions */
START_TEST(test_chip8_pc) {

    ck_assert_uint_eq(chip8_pc_get(c), c->pc);

    ASSERT_PC(PROGRAM_START)
    chip8_pc_incr(c);
    ASSERT_PC(PROGRAM_START + 2)
    chip8_pc_set(c, 0xffe);
    ASSERT_PC(0xffe)
    chip8_pc_incr(c);
    ASSERT_PC(0x000)

} END_TEST

/* checks register functions */
START_TEST(test_chip8_reg) {
    uint8_t i;

    for (i=0; i<NUM_REGS; i++) {
        chip8_reg_set(c,i,0xab);
        ck_assert_uint_eq(chip8_reg_get(c,i), c->V[i]);
        ck_assert_uint_eq(chip8_reg_get(c,i), 0xab);
    }

} END_TEST



Suite* chip8_suite(void) {

    TCase* tc_core = tcase_create("core");
    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_chip8_init);
    tcase_add_test(tc_core, test_chip8_pc);
    tcase_add_test(tc_core, test_chip8_reg);

    Suite* s = suite_create("chip8");
    suite_add_tcase(s, tc_core);

    return s;
}

