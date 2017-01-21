# chip-8 [![Build Status](https://travis-ci.org/lofaldli/chip-8.svg?branch=master)](https://travis-ci.org/lofaldli/chip-8)

this is a [chip-8][1] emulator written in C inspired by [these][2] [guys][3]

also included is a simple assembler written in python 

[1]: https://en.wikipedia.org/wiki/CHIP-8
[2]: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
[3]: http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

## dependencies

- `libsdl1.2-dev`
- `libsdl-gfx1.2-dev`
- `check`

## build instructions (cmake)

        mkdir build/
        cd build/
        cmake ..
        make

## instruction set

        0nnn - SYS  addr    : (unused)
        00E0 - CLS          : clears the display
        00EE - RET          : return from a subroutine
        1nnn - JMP  addr    : jump to addr
        2nnn - CALL addr    : call subroutine at addr
        3xkk - SEQ  Vx kk   : skip next instruction if Vx == kk
        4xkk - SNE  Vx kk   : skip next instruction if Vx != kk
        5xy0 - SEQ  Vx Vy   : skip next instruction if Vx == Vy
        6xkk - LD   Vx kk   : Vx = kk
        7xkk - ADD  Vx kk   : Vx = Vx + kk
        8xy0 - LD   Vx Vy   : Vx = Vy
        8xy1 - AND  Vx Vy   : Vx = Vx & Vy
        8xy2 - OR   Vx Vy   : Vx = Vx | Vy
        8xy3 - XOR  Vx Vy   : Vx = Vx ^ Vy
        8xy4 - ADD  Vx Vy   : Vx = Vx + Vy
        8xy5 - SUB  Vx Vy   : Vx = Vx - Vy
        8xy6 - SHR  Vx Vy   : Vx = Vx SHR 1
        8xy7 - SUBN Vx Vy   : Vx = Vy - Vx
        8xyE - SHL  Vx Vy   : Vx = Vx SHL 1
        9xy0 - SNE  Vx Vy   : skip next instruction if Vx != Vy
        Annn - LD   I  addr : I = addr
        Bnnn - JMP  V0 addr : jump to V0 + addr
        Cxkk - RND  Vx kk   : Vx = rand() & kk
        Dxyn - DRAW Vx Vy n : draw sprite at Vx,Vy from location I of size n
        Ex9E - SKP  Vx      : skip next instruction if key Vx is pressed
        ExA1 - SKNP Vx      : skip next instruction if key Vx is not pressed
        Fx07 - LD   Vx DT   : Vx = delay timer value
        Fx0A - LD   K  Vx   : wait for key press and store key in Vx
        Fx15 - LD   DT Vx   : delay timer = Vx
        Fx18 - LD   ST Vx   : sound timer = Vx
        Fx1E - ADD  I  Vx   : I = I + Vx
        Fx29 - LD   F  Vx   : I = location of sprite for digit in Vx
        Fx33 - LD   B  Vx   : BCD representation of Vx stored in I, I+1 and I+2
        Fx55 - LD   [I] Vx  : store registers V0 to Vx in memory starting at I
        Fx65 - LD   Vx [I]  : write registers V0 to Vx from memory starting at I
