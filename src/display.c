#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include "display.h"

display* display_init(uint8_t width, uint8_t height) {
    display* d = malloc(sizeof(display));

    d->width = width, d->height = height;

    SDL_Init( SDL_INIT_VIDEO );
    d->screen = SDL_SetVideoMode(
            width*PIXEL_SIZE, height*PIXEL_SIZE, 0,
            SDL_HWSURFACE | SDL_DOUBLEBUF );
    SDL_WM_SetCaption("chip-8", 0);

    return d;
}

void display_free(display* d) {
    free(d);
    SDL_Quit();
}

void display_draw(display* d, uint8_t* buffer) {
    for (uint8_t y=0; y<d->height; y++) {
        for (uint8_t x=0; x<d->width; x++) {
            if ((buffer[x + y*d->width] & 0x01) == 0x01)
                boxRGBA(d->screen,
                        x*PIXEL_SIZE, y*PIXEL_SIZE,
                        (x+1)*PIXEL_SIZE, (y+1)*PIXEL_SIZE,
                        0xFF, 0xFF, 0xFF, 0xFF);
            else
                boxRGBA(d->screen,
                        x*PIXEL_SIZE, y*PIXEL_SIZE,
                        (x+1)*PIXEL_SIZE, (y+1)*PIXEL_SIZE,
                        0x00, 0x00, 0x00, 0xFF);

        }
    }
    SDL_Flip(d->screen);
}
