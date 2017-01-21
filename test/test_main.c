#include "test_chip8.h"


int main(void) {

    SRunner* sr = srunner_create(chip8_suite());
    srunner_add_suite(sr, opcode_suite());

    srunner_run_all(sr, CK_NORMAL);

    int n_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (n_failed == 0) ? 0 : 1;

}
