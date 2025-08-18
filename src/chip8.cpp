#include "chip8.h"
#include <cstdint>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

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

  std::cout << "Chip8 initialized" << std::endl;
}

void Chip8::logOpcode(const std::string &message) {
  std::cout << "Opcode: " << std::hex << opcode << " " << message << std::endl;
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
      {OpcodeType::SET_I, std::bind(&Chip8::set_i, this)},
      {OpcodeType::SET_PC_NNN_PLUS_V0,
       std::bind(&Chip8::set_pc_nnn_plus_v0, this)},
      {OpcodeType::RANDOM, std::bind(&Chip8::random, this)},
      {OpcodeType::DRAW, std::bind(&Chip8::draw, this)},
      {OpcodeType::KEYPAD, std::bind(&Chip8::keypad, this)},
      {OpcodeType::TIMERS, std::bind(&Chip8::timers, this)},
  };

  OpcodeType instruction_type = static_cast<OpcodeType>(opcode & 0xF000);
  auto func = instruction_type_map.find(instruction_type)->second;
  if (!func) {
    std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
    exit(1);
  }

  func();

  if (delay_timer > 0)
    delay_timer--;
  if (sound_timer > 0) {
    sound_timer--;
    if (sound_timer == 0)
      std::cout << "BEEP!" << std::endl;
  }
}

void Chip8::display() {
  switch (opcode) {
  case 0x00E0:
    logOpcode("Clear the screen");
    gfx.fill(0);
    drawFlag = true;
    pc += 2;
    return;
  case 0x00EE:
    // Return from subroutine so we substract from the stack pointer and set the
    // pc to be the value at the top of the stack Which is the address of the
    // next instruction to execute
    logOpcode("Return from subroutine");
    pc = stack[--sp];
    pc += 2;
    return;
  default:
    std::cerr << "Unknown 0x0000 opcode: " << std::hex << opcode << std::endl;
  }
}

void Chip8::jump() {
  uint16_t address = opcode & 0x0FFF;
  logOpcode("Jump to address: " + std::to_string(address));
  pc = address;
}

void Chip8::call() {
  uint16_t address = opcode & 0x0FFF;
  logOpcode("Call subroutine: " + std::to_string(address));
  // Push the current PC to the stack and then set the PC to the address
  stack[sp++] = pc;
  pc = address;
}

void Chip8::skip_equal() {
  uint8_t value = opcode & 0xFF;
  uint8_t registerIdx = (opcode & 0xF00) >> 8;
  logOpcode("Skip if V" + std::to_string(registerIdx) +
            " == " + std::to_string(value));
  if (V[registerIdx] == value)
    pc += 4;
  else
    pc += 2;
}

void Chip8::skip_not_equal() {
  uint8_t value = opcode & 0xFF;
  uint8_t registerIdx = (opcode & 0xF00) >> 8;
  logOpcode("Skip if V" + std::to_string(registerIdx) +
            " != " + std::to_string(value));
  if (V[registerIdx] != value)
    pc += 4;
  else
    pc += 2;
}

void Chip8::skip_equal_vx_vy() {
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t y = (opcode & 0x0F0) >> 4;
  logOpcode("Skip if V" + std::to_string(x) + " == V" + std::to_string(y));
  if (V[x] == V[y])
    pc += 4;
  else
    pc += 2;
}

void Chip8::set_vx() {
  uint8_t value = opcode & 0xFF;
  uint8_t registerIdx = (opcode & 0xF00) >> 8;
  logOpcode("Set V" + std::to_string(registerIdx) + " = " +
            std::to_string(value));
  V[registerIdx] = value;
  pc += 2;
}

void Chip8::add_vx() {
  uint8_t value = opcode & 0xFF;
  uint8_t registerIdx = (opcode & 0xF00) >> 8;
  logOpcode("Add " + std::to_string(value) + " to V" +
            std::to_string(registerIdx));
  V[registerIdx] += value;
  pc += 2;
}

void Chip8::alu_vx_vy() {
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t y = (opcode & 0x0F0) >> 4;
  uint8_t operation = opcode & 0x00F;

  switch (operation) {
  case (0x0): {
    logOpcode("Set Vx = Vy");
    V[x] = V[y];
    break;
  }
  case (0x1): {
    logOpcode("Set Vx |= Vy");
    V[x] |= V[y];
    break;
  }
  case (0x2): {
    logOpcode("Set Vx &= Vy");
    V[x] &= V[y];
    break;
  }
  case (0x3): {
    logOpcode("Set Vx ^= Vy");
    V[x] ^= V[y];
    break;
  }
  case (0x4): {
    logOpcode("Set Vx = Vx + Vy and Vf = carry");
    uint16_t sum = V[x] + V[y];
    V[0xF] = (sum > 255) ? 1 : 0;
    V[x] = sum & 0xFF;
    break;
  }
  case (0x5): {
    logOpcode("Set Vx = Vx - Vy and Vf = V[x] > V[y]");
    V[0xF] = (V[x] > V[y]) ? 1 : 0; // Store in Vf if not borrowed
    V[x] -= V[y];
    break;
  }
  case (0x6): {
    logOpcode("Set Vx >>= 1 and Vf = Vx & 0x1");
    V[0xF] = V[x] & 0x1; // Store the dropped bit in Vf
    V[x] >>= 1;
    break;
  }
  case (0x7): {
    logOpcode("Set Vx = Vy - Vx and Vf = Vy > Vx");
    V[0xF] = (V[y] > V[x]) ? 1 : 0; // Store in Vf if not borrowed
    V[x] = V[y] - V[x];
    break;
  }
  case (0xE): {
    logOpcode("Set Vx <<= 1 and Vf = (Vx & 0x80) >> 7");
    V[0xF] = (V[x] & 0x80) >> 7; // Store the dropped bit in Vf (the 7th one)
    V[x] <<= 1;
    break;
  }
  }

  pc += 2;
}

void Chip8::skip_not_equal_vx_vy() {
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t y = (opcode & 0x0F0) >> 4;
  logOpcode("Skip if V" + std::to_string(x) + " != V" + std::to_string(y));
  if (V[x] != V[y])
    pc += 4;
  else
    pc += 2;
}

void Chip8::set_i() {
  uint16_t value = opcode & 0xFFF;
  logOpcode("Set I = " + std::to_string(value));
  I = value;
  pc += 2;
}

void Chip8::set_pc_nnn_plus_v0() {
  uint16_t value = opcode & 0xFFF;
  logOpcode("Set PC = V0 + " + std::to_string(value));
  pc = value + V[0x0];
}

void Chip8::random() {
  uint8_t value = opcode & 0xFF;
  uint8_t registerIdx = (opcode & 0xF00) >> 8;
  logOpcode("Set V" + std::to_string(registerIdx) + " to random");
  V[registerIdx] = (rand() % 256) & value;
  pc += 2;
}

void Chip8::draw() {
  uint8_t x = V[(opcode & 0xF00) >> 8];
  uint8_t y = V[(opcode & 0x0F0) >> 4];
  uint8_t height = opcode & 0x00F;

  logOpcode("Draw x: " + std::to_string(x) + ", y: " + std::to_string(y) + ", height: " + std::to_string(height) );

  V[0xF] = 0;

  for (int row = 0; row < height; row++) {
    uint8_t sprite = memory[I + row];
    for (int col = 0; col < SPRITE_COLS; col++) {
      if ((sprite & (0x80 >> col)) != 0) {
        int index = (x + col + ((y + row) * VIDEO_WIDTH)) % VIDEO_SIZE;
        if (gfx[index] == 1)
          V[0xF] = 1;
        gfx[index] ^= 1;
      }
    }
  }

  drawFlag = true;
  pc += 2;
}

void Chip8::keypad() {
  uint8_t operation = opcode & 0xFF;
  uint8_t x = (opcode & 0xF00) >> 8;
  uint8_t key = keys[V[x]];

  switch (operation) {
  case 0x9E: {
    logOpcode("Key " + std::to_string(key) + " pressed, skipping");
    if (key)
      pc += 4;
    else
      pc += 2;
    return;
  }
  case 0xA1: {
    logOpcode("Key " + std::to_string(key) + " not pressed, skipping");
    if (!key)
      pc += 4;
    else
      pc += 2;
    return;
  }
  default: {
    std::cerr << "Unknown 0xE000 opcode: " << std::hex << opcode << std::endl;
    pc += 2;
    return;
  }
  }
}

void Chip8::timers() {
  uint8_t operation = opcode & 0xFF;
  uint8_t x = (opcode & 0xF00) >> 8;

  switch (operation) {
  case 0x07: {
    logOpcode("Set V" + std::to_string(x) + " to delay_timer");
    V[x] = delay_timer;
    break;
  }
  case 0x0A: {
    logOpcode("Wait for keypress and store the key index in V" +
              std::to_string(x));
    bool key_pressed = false;

    for (int keyIdx = 0; keyIdx < KEYPAD_SIZE; keyIdx++) {
      if (!keys[keyIdx])
        continue;

      V[x] = keyIdx;
      key_pressed = true;
      break;
    }

    if (key_pressed)
      return; // Wait

    break;
  }
  case 0x15: {
    logOpcode("Set delay_timer to V" + std::to_string(x));
    delay_timer = V[x];
    break;
  }
  case 0x18: {
    logOpcode("Set sound_timer to V" + std::to_string(x));
    sound_timer = V[x];
    break;
  }
  case 0x1E: {
    logOpcode("Set I += V" + std::to_string(x));
    I += V[x];
    break;
  }
  case 0x29: {
    logOpcode("Set I to the location of the sprite for digit V" +
              std::to_string(x));
    // TODO: investigate
    I = V[x] * 5;
    break;
  }
  case 0x33: {
    memory[I] = V[x] / 100;
    memory[I + 1] = (V[x] / 10) % 10;
    memory[I + 2] = V[x] % 10;
    logOpcode("Store BCD representation of V" + std::to_string(x) + "(" +
              std::to_string(V[x]) + ") in memory I, I+1 and I+2 " +
              std::to_string(memory[I]) + " " + std::to_string(memory[I + 1]) +
              " " + std::to_string(memory[I + 2]));
    break;
  }
  case 0x55: {
    logOpcode("Store V0 to V" + std::to_string(x) + " in memory starting at I");
    memcpy(&memory[I], V.data(), x);
    break;
  }
  case 0x65: {
    logOpcode("Load memory starting at I into V0 to V" + std::to_string(x));
    memcpy(V.data(), &memory[I], x);
    break;
  }
  default: {
    std::cerr << "Unknown 0xE000 opcode: " << std::hex << opcode << std::endl;
  }
  }

  pc += 2;
}