#include <array>

#define KEYPAD_SIZE 16
#define REGISTER_COUNT 16
#define MEMORY_SIZE 4096
#define STACK_SIZE 16
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32
#define VIDEO_SIZE (VIDEO_WIDTH * VIDEO_HEIGHT)

class Chip8 {
    public:
        std::array<uint8_t, VIDEO_SIZE> gfx;
        std::array<uint8_t, KEYPAD_SIZE> keys;

    private:
        std::array<uint8_t, MEMORY_SIZE> memory;
        std::array<uint8_t, REGISTER_COUNT> V;
        std::array<uint16_t, STACK_SIZE> stack;

        uint16_t opcode;
        uint16_t sp;
        uint8_t delay_timer;
        uint8_t sound_timer;
        uint16_t I; // Index register
        uint16_t pc; // Program counter
};