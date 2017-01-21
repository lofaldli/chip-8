#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL/SDL.h>

#include "chip8.h"
#include "display.h"

int debug = 0;
int dump = 0;
char* filename = "games/demo.c8";
SDL_Event event;

uint8_t scancodes[NUM_KEYS] = {
    0x0a, 0x0b, 0x0c, 0x0d, // 1 2 3 4
    0x18, 0x19, 0x1a, 0x1b, // q w e r
    0x26, 0x27, 0x28, 0x29, // a s d f
    0x34, 0x35, 0x36, 0x37  // z x c v
};

uint8_t key_map[NUM_KEYS] = {
    0x01, 0x02, 0x03, 0x0c, // 1 2 3 c
    0x04, 0x05, 0x06, 0x0d, // 4 5 6 d
    0x07, 0x08, 0x09, 0x0e, // 7 8 9 e
    0x0a, 0x00, 0x0b, 0x0f  // a 0 b f
};

int parse_args(int argc, char** argv) {
    int c;
    while ((c = getopt(argc, argv, "dm")) != -1) {
        switch (c) {
            case 'd':
                debug = 1;
                break;
            case 'm':
                dump = 1;
                break;
            case '?':
                printf("%s", optarg);
            default:
                return 1;
        }
    }
    if (optind < argc) {
        filename = argv[optind];
    }
    return 0;

}

int main(int argc, char** argv) {

    if (parse_args(argc, argv) != 0) {
        return 1;
    }

    chip8* c = chip8_init();

    if (chip8_program_load(c, filename) != 0) {
        chip8_free(c);
        return 1;
    }

    if (dump)
        chip8_mem_dump(c);

    display* d = display_init(WIDTH, HEIGHT);

    int running = 1;

    while (running) {
        if (!chip8_check_flag(c,HALT)) {

            chip8_emulate_cycle(c);

            if (debug)
                chip8_debug_print(c);

            if (chip8_check_flag(c,DRAW)) {
                display_draw(d, c->gfx);
                c->flags ^= DRAW;
            }

        }

        while ( SDL_PollEvent(&event) ) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_KEYUP:
                case SDL_KEYDOWN:
                    for (uint8_t i=0; i<NUM_KEYS; i++) {
                        if (event.key.keysym.scancode == scancodes[i]) {
                            uint8_t state = (event.type == SDL_KEYDOWN) ? 1 : 0;
                            chip8_key_set(c, key_map[i], state);
                            break;
                        }
                    }
                    break;
                default:
                    break;
            }

        }
            if (event.type == SDL_QUIT)
                running = 0;

        SDL_Delay(1);
    }

    chip8_free(c);
    display_free(d);

    return 0;
}
