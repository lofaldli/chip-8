#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <SDL/SDL.h>

#define PIXEL_SIZE 10

struct display_t {
    SDL_Surface* screen;
    uint8_t width, height;
} display_t;

typedef struct display_t display;

display* display_init(uint8_t width, uint8_t height);
void display_free(display* d);
void display_draw(display* d, uint8_t* buffer);
void display_delay(display* d);
void display_event(display* d);

#endif
