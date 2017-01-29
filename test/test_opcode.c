#include <check.h>
#include "test_chip8.h"

static chip8* c;
static void setup() {
    c = chip8_init();
}
static void teardown() {
    chip8_free(c);
}

START_TEST(test_flow_subroutine) {

    uint16_t pc = chip8_pc_get(c);

    EXEC(0x2abc) /* CALL 0xabc*/
    ASSERT_PC(0xabc)
    ck_assert_uint_eq(c->sp, 1);
    ck_assert_uint_eq(c->stack[0], pc);

    EXEC(0x00ee) /* ret */
    ASSERT_PC(pc)
    ck_assert_uint_eq(c->sp, 0);

} END_TEST

START_TEST(test_flow_skip) {
    uint16_t pc;

    pc = chip8_pc_get(c); c->V[0] = 0xab;
    EXEC(0x30ab) /* SEQ V0 0xab */
    ASSERT_PC(pc+4)

    pc = chip8_pc_get(c); c->V[0] = 0xab;
    EXEC(0x30ff) /* SEQ V0 0xff */
    ASSERT_PC(pc+2)

    pc = chip8_pc_get(c); c->V[0] = 0xab;
    EXEC(0x40ab) /* SNE V0 0xab */
    ASSERT_PC(pc+2)

    pc = chip8_pc_get(c); c->V[0] = 0xab;
    EXEC(0x40ff) /* SNE V0 0xff */
    ASSERT_PC(pc+4)

    pc = chip8_pc_get(c);
    c->V[0] = 0xab; c->V[1] = 0xab;
    EXEC(0x5010) /* SEQ V0 V1 */
    ASSERT_PC(pc+4)

    pc = chip8_pc_get(c);
    c->V[0] = 0xab; c->V[1] = 0xbc;
    EXEC(0x5010) /* SEQ V0 V1 */
    ASSERT_PC(pc+2)

    pc = chip8_pc_get(c);
    c->V[0] = 0xab; c->V[1] = 0xab;
    EXEC(0x9010) /* SEQ V0 V1 */
    ASSERT_PC(pc+2)

    pc = chip8_pc_get(c);
    c->V[0] = 0xab; c->V[1] = 0xbc;
    EXEC(0x9010) /* SEQ V0 V1 */
    ASSERT_PC(pc+4)

} END_TEST

START_TEST(test_flow_skip_keys) {
    uint16_t pc;

    for (uint8_t key=0; key<NUM_KEYS; key++) {
        pc = chip8_pc_get(c);
        c->V[0] = key; c->keys[key] = 0;
        EXEC(0xe09e) ASSERT_PC(pc+2)

        pc = chip8_pc_get(c);
        c->V[0] = key; c->keys[key] = 1;
        EXEC(0xe09e) ASSERT_PC(pc+4)
    }

} END_TEST

START_TEST(test_flow_jump) {

    EXEC(0x1abc) ASSERT_PC(0xabc)
    c->V[0] = 0xab;
    EXEC(0xBcde) ASSERT_PC(0xab + 0xcde)

} END_TEST

START_TEST(test_math_add) {

    c->V[0] = 0x12;
    EXEC(0x7034); /* ADD V0 0x34 */
    ASSERT_REG(0, 0x12 + 0x34)
    ASSERT_REG(0xf, 0)

    c->V[0] = 0x12;
    EXEC(0x70ff); /* ADD V0 0xff */
    ASSERT_REG(0, 0x12 + 0xff)
    ASSERT_REG(0xf, 1)

    c->V[0] = 0x12; c->V[1] = 0x34;
    EXEC(0x8014); /* ADD V0 V1 */
    ASSERT_REG(0, 0x12 + 0x34)
    ASSERT_REG(0xf, 0)

    c->V[0] = 0x12; c->V[1] = 0xff;
    EXEC(0x8014); /* ADD V0 V1 */
    ASSERT_REG(0, 0x12 + 0xff)
    ASSERT_REG(0xf, 1)

    c->I = 0x123; c->V[0] = 0x45;
    EXEC(0xf01e) /* ADD I V0 */
    ck_assert_uint_eq(c->I, 0x123 + 0x45);

} END_TEST


START_TEST(test_math_sub) {

    c->V[0] = 0x43; c->V[1] = 0x21;
    EXEC(0x8015); /* SUB V0 V1 */
    ASSERT_REG(0, 0x43 - 0x21)
    ASSERT_REG(0xf, 0)

    c->V[0] = 0x12; c->V[1] = 0x34;
    EXEC(0x8015); /* SUB V0 V1 */
    ASSERT_REG(0, 0x12 - 0x34)
    ASSERT_REG(0xf, 1)

    c->V[0] = 0x34; c->V[1] = 0x12;
    EXEC(0x8017); /* SUBN V0 V1 */
    ASSERT_REG(0, 0x12-0x34)
    ASSERT_REG(0xf, 1)

    c->V[0] = 0x21; c->V[1] = 0x43;
    EXEC(0x8017); /* SUBN V0 V1 */
    ASSERT_REG(0, 0x43-0x21)
    ASSERT_REG(0xf, 0)

} END_TEST

START_TEST(test_math_logic) {

    c->V[0] = 0x12; c->V[1] = 0x34;
    EXEC(0x8011) /* AND V0 V1 */
    ASSERT_REG(0, 0x12 & 0x34)

    c->V[0] = 0x12; c->V[1] = 0x34;
    EXEC(0x8012) /* OR V0 V1 */
    ASSERT_REG(0, 0x12 | 0x34)

    c->V[0] = 0x12; c->V[1] = 0x34;
    EXEC(0x8013) /* XOR V0 V1 */
    ASSERT_REG(0, 0x12 ^ 0x34)

} END_TEST

START_TEST(test_load_registers) {

    EXEC(0x60ab) /* V0 = 0xab */
    ASSERT_REG(0,0xab)

    c->V[0] = 0; c->V[1] = 0xff;
    EXEC(0x8010) /* V0 = V1 */
    ASSERT_REG(0, 0xff)

    c->I = 0x200;
    EXEC(0xafff) /* I = 0xfff */
    ck_assert_uint_eq(c->I, 0xfff);

} END_TEST

START_TEST(test_load_timers) {

    c->V[0] = 0xab;
    EXEC(0xf015) /* delay timer = V0 */
    ck_assert_uint_eq(c->delay_timer, 0xab);
    EXEC(0xf018) /* sound timer = V0 */
    ck_assert_uint_eq(c->sound_timer, 0xab);
    c->V[0] = 0;
    EXEC(0xf007) /* V0 = delay timer */
    ASSERT_REG(0, 0xab);

} END_TEST

START_TEST(test_load_bcd) {
    c->V[0] = 0xab;
    EXEC(0xf033)
    ck_assert_uint_eq(c->memory[c->I+0], 0xab / 100);
    ck_assert_uint_eq(c->memory[c->I+1], (0xab % 100) / 10);
    ck_assert_uint_eq(c->memory[c->I+2], (0xab % 10));
} END_TEST

START_TEST(test_load_memory) {
    c->V[0] = 0xde; c->V[1] = 0xad;
    c->V[2] = 0xbe; c->V[3] = 0xef;
    c->I = 0x123;
    EXEC(0xf355) /* store V0-V3 in memory starting at I */
    ck_assert_uint_eq(c->memory[c->I+0], 0xde);
    ck_assert_uint_eq(c->memory[c->I+1], 0xad);
    ck_assert_uint_eq(c->memory[c->I+2], 0xbe);
    ck_assert_uint_eq(c->memory[c->I+3], 0xef);

    c->V[0] = 0x00; c->V[1] = 0x00;
    c->V[2] = 0x00; c->V[3] = 0x00;
    EXEC(0xf365) /* write V0-V3 from memory starting at I */
    ASSERT_REG(0, 0xde)
    ASSERT_REG(1, 0xad)
    ASSERT_REG(2, 0xbe)
    ASSERT_REG(3, 0xef)
} END_TEST

Suite* opcode_suite(void) {
    TCase* tc_flow = tcase_create("program flow");
    tcase_add_checked_fixture(tc_flow, setup, teardown);
    tcase_add_test(tc_flow, test_flow_subroutine);
    tcase_add_test(tc_flow, test_flow_skip);
    tcase_add_test(tc_flow, test_flow_skip_keys);
    tcase_add_test(tc_flow, test_flow_jump);

    TCase* tc_math = tcase_create("math");
    tcase_add_checked_fixture(tc_math, setup, teardown);
    tcase_add_test(tc_math, test_math_add);
    tcase_add_test(tc_math, test_math_sub);
    tcase_add_test(tc_math, test_math_logic);

    TCase* tc_load = tcase_create("load");
    tcase_add_checked_fixture(tc_load, setup, teardown);
    tcase_add_test(tc_load, test_load_registers);
    tcase_add_test(tc_load, test_load_timers);
    tcase_add_test(tc_load, test_load_bcd);
    tcase_add_test(tc_load, test_load_memory);

    Suite* s = suite_create("opcode");
    suite_add_tcase(s, tc_flow);
    suite_add_tcase(s, tc_math);
    suite_add_tcase(s, tc_load);

    return s;
}
