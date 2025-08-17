// Example program:
// Using SDL3 to create an application window

#include "chip8.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <thread>
#include <iostream>
#include <chrono>

int main(int argc, char *argv[]) {
  Chip8 chip8 = Chip8();

  SDL_Window *window; // Declare a pointer
  bool done = false;

  SDL_Init(SDL_INIT_VIDEO); // Initialize SDL3

  // Create an application window with the following settings:
  window = SDL_CreateWindow("Chip8 emulator", // window title
                            VIDEO_WIDTH,              // width, in pixels
                            VIDEO_HEIGHT,              // height, in pixels
                            SDL_WINDOW_OPENGL // flags - see below
  );

  // Check that the window was successfully created
  if (window == NULL) {
    // In the case that the window could not be made...
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n",
                 SDL_GetError());
    return 1;
  }

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
  }

  // Close and destroy the window
  SDL_DestroyWindow(window);

  // Clean up
  SDL_Quit();
  return 0;
}