#include <array>
#include <cstdint>
#include <stdbool.h>
#include <string>

#define KEYPAD_SIZE 16
#define REGISTER_COUNT 16
#define MEMORY_SIZE 4096
#define STACK_SIZE 16
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32
#define VIDEO_SIZE (VIDEO_WIDTH * VIDEO_HEIGHT)
#define FONTSET_MEMORY_START 0x0
#define ROM_MEMORY_START 0x200
#define SPRITE_COLS 8

enum class OpcodeType {
  DISPLAY = 0x0,
  JUMP = 0x1000,
  CALL = 0x2000,
  SKIP_EQUAL = 0x3000,
  SKIP_NOT_EQUAL = 0x4000,
  SKIP_EQUAL_VX_VY = 0x5000,
  SET_VX = 0x6000,
  ADD_VX = 0x7000,
  ALU_VX_VY = 0x8000,
  SKIP_NOT_EQUAL_VX_VY = 0x9000,
  SET_I = 0xA000,
  SET_PC_NNN_PLUS_V0 = 0xB000,
  RANDOM = 0xC000,
  DRAW = 0xD000,
};

class Chip8 {
public:
  // 2.5 Functions
  Chip8();
  // 3.3 ROM loader
  void loadROM(const std::string &path);
  void emulateCycle();

  std::array<uint8_t, VIDEO_SIZE> gfx;
  std::array<uint8_t, KEYPAD_SIZE> keys;

private:
  void initialize();
  void loadFontset();

  void logOpcode(const std::string &message);

  void display();
  void jump();
  void call();
  void skip_equal();
  void skip_not_equal();
  void skip_equal_vx_vy();
  void set_vx();
  void add_vx();
  void alu_vx_vy();
  void skip_not_equal_vx_vy();
  void set_i();
  void set_pc_nnn_plus_v0();
  void random();
  void draw();

  // 2.3 Array for features
  std::array<uint8_t, MEMORY_SIZE> memory;
  std::array<uint8_t, REGISTER_COUNT> V;
  std::array<uint16_t, STACK_SIZE> stack;

  // 2.4 Other features
  uint16_t opcode;
  uint16_t sp;
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint16_t I;  // Index register
  uint16_t pc; // Program counter
  bool drawFlag;

  // 3.2 Fonts: each for is 5 bytes and each byte represents a row of the font
  // For example the 0 is 0xF0, 0x90, 0x90, 0x90, 0xF0,
  // which represents the following:
  // 0xF0 = 11110000
  // 0x90 = 10010000
  // 0x90 = 10010000
  // 0x90 = 10010000
  // 0xF0 = 11110000
  uint8_t fontset[80] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0x90, // 8
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // 9
      0xF0, 0x80, 0xF0, 0x80, 0x80, // A
      0xE0, 0x90, 0x90, 0x90, 0xE0, // B
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };
};