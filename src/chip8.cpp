#include "chip8.h"
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <unordered_map>
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

  emulateCycle();
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
    exit(1);
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

void Chip8::emulateCycle() {
  opcode = memory[pc] << 8 | memory[pc + 1];
  opcode = 0x00E0;

  std::unordered_map<OpcodeType, std::function<void()>> instruction_type_map = {
      {OpcodeType::DISPLAY, std::bind(&Chip8::display, this)},
      {OpcodeType::JUMP, std::bind(&Chip8::jump, this)},
      {OpcodeType::CALL, std::bind(&Chip8::call, this)},
      {OpcodeType::SKIP_EQUAL, std::bind(&Chip8::skip_equal, this)},
      {OpcodeType::SKIP_NOT_EQUAL, std::bind(&Chip8::skip_not_equal, this)},
      {OpcodeType::SKIP_EQUAL_VX_VY, std::bind(&Chip8::skip_equal_vx_vy, this)},
      {OpcodeType::SET_VX, std::bind(&Chip8::set_vx, this)},
      {OpcodeType::ADD_VX, std::bind(&Chip8::add_vx, this)},
      {OpcodeType::ALU_VX_VY, std::bind(&Chip8::alu_vx_vy, this)},
      {OpcodeType::SKIP_NOT_EQUAL_VX_VY,
       std::bind(&Chip8::skip_not_equal_vx_vy, this)},
      {OpcodeType::SET_I, std::bind(&Chip8::set_i, this)}};

  OpcodeType instruction_type = static_cast<OpcodeType>(opcode & 0xF000);
  auto func = instruction_type_map.find(instruction_type)->second;
  if (!func) {
    std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
    exit(1);
  }

  func();
}

void Chip8::display() {
  switch (opcode) {
    case 0x00E0:
      std::cout << "Opcode 0x00E0: Clear the screen" << std::endl;
      gfx.fill(0);
      drawFlag = true;
      pc += 2;
      return;
    case 0x00EE:
      // Return from subroutine so we substract from the stack pointer and set the pc to be the value at the top of the stack
      // Which is the address of the next instruction to execute
      std::cout << "Opcode 0x00EE: Return from subroutine" << std::endl;
      pc = stack[--sp];
      return;
    default:
      std::cerr << "Unknown 0x0000 opcode: " << std::hex << opcode << std::endl;
  }
}

void Chip8::jump() {
}

void Chip8::call() {
}

void Chip8::skip_equal() {
}

void Chip8::skip_not_equal() {
}

void Chip8::skip_equal_vx_vy() {
}

void Chip8::set_vx() {
}

void Chip8::add_vx() {
}

void Chip8::alu_vx_vy() {
}

void Chip8::skip_not_equal_vx_vy() {
}

void Chip8::set_i() {
}
