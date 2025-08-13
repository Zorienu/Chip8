#include "chip8.h"
#include <iostream>
#include <fstream>
#include <vector>

Chip8::Chip8() {
    initialize();
}

/**
 * Clears memory, registers, timers and resets everything to the default state.
 */
void Chip8::initialize() {
    pc = 0x200; // Program counter set to 0x200 where the game starts
    opcode = 0;
    I = 0;
    sp = 0;

    memory.fill(0);
    V.fill(0);
    stack.fill(0);
    gfx.fill(0);
    keys.fill(0);

    delay_timer = 0;
    sound_timer = 0;
}