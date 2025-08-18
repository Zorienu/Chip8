// Example program:
// Using SDL3 to create an application window

#include "chip8.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <thread>

const SDL_Scancode keymap[KEYPAD_SIZE] = {
    SDL_SCANCODE_X, SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_A,
    SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_Z, SDL_SCANCODE_C,
    SDL_SCANCODE_4, SDL_SCANCODE_R, SDL_SCANCODE_F, SDL_SCANCODE_V};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: chip8 <ROM file>" << std::endl;
    return 1;
  }

  Chip8 chip8 = Chip8();
  chip8.loadROM(argv[1]);

  SDL_Window *window; // Declare a pointer
  bool done = false;

  SDL_Init(SDL_INIT_VIDEO); // Initialize SDL3

  // Create an application window with the following settings:
  window = SDL_CreateWindow("Chip8 emulator",           // window title
                            VIDEO_WIDTH * VIDEO_SCALE,  // width, in pixels
                            VIDEO_HEIGHT * VIDEO_SCALE, // height, in pixels
                            SDL_WINDOW_OPENGL           // flags
  );

  // Check that the window was successfully created
  if (window == NULL) {
    // In the case that the window could not be made...
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n",
                 SDL_GetError());
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

  while (!done) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }

      if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        bool pressed = event.type == SDL_EVENT_KEY_DOWN;
        SDL_Scancode scancode = event.key.scancode;
        for (int i = 0; i < KEYPAD_SIZE; i++) {
          if (keymap[i] == scancode) {
            chip8.keys[i] = pressed;
            break;
          }
        }

        for (int i = 0; i < KEYPAD_SIZE; i++) {
          std::cout << std::to_string(chip8.keys[i]) << " ";
        }
        std::cout << std::endl;
      }
    }

    // Do game logic, present a frame, etc.
    chip8.emulateCycle();

    if (chip8.drawFlag) {
      chip8.drawFlag = false;

      SDL_FRect rect = {.x = 0, .y = 0, .w = VIDEO_SCALE, .h = VIDEO_SCALE};

      for (int pixelIdx = 0; pixelIdx < VIDEO_WIDTH * VIDEO_HEIGHT;
           pixelIdx++) {
        rect.x = (pixelIdx % VIDEO_WIDTH) * VIDEO_SCALE;
        rect.y = (pixelIdx / VIDEO_WIDTH) * VIDEO_SCALE;
        if (chip8.gfx[pixelIdx]) {
          SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
          SDL_RenderFillRect(renderer, &rect);
        } else {
          SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
          SDL_RenderFillRect(renderer, &rect);
        }
      }

      SDL_RenderPresent(renderer);
    }
    std::this_thread::sleep_for(std::chrono::microseconds(1500));
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}