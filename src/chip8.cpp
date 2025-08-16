#include "chip8.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>


Chip8::Chip8() { initialize(); }

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

  loadFontset();

  loadROM("./games/PONG");

  std::cout << "Chip8 initialized" << std::endl;
}

void Chip8::loadFontset() {
  memcpy(memory.data() + FONTSET_MEMORY_START, fontset, sizeof(fontset));

  std::cout << "Fontset loaded" << std::endl;
  std::cout << "Fontset memory start: " << std::hex << FONTSET_MEMORY_START
            << std::endl;
  std::cout << "Fontset memory size: " << sizeof(fontset) << std::endl;
}

void Chip8::loadROM(const std::string &path) {
  std::ifstream file(path, std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    std::cerr << "Failed to open ROM file: " << path << std::endl;
    return;
  }

  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  file.read((char *)memory.data() + ROM_MEMORY_START, size);
  file.close();

  std::cout << "ROM loaded" << std::endl;
  std::cout << "ROM memory start: " << std::hex << ROM_MEMORY_START
            << std::endl;
  std::cout << "ROM memory size: " << size << std::endl;
  std::cout << "ROM memory first byte: " << std::hex
            << (int)memory[ROM_MEMORY_START + 1] << std::endl;
}