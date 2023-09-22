#include "main.hpp"

#include <SDL2/SDL.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>

#include "gui/gui_layout.hpp"
#include "gui/lib/cursors.hpp"
#include "gui/lib/icons.hpp"
#include "gui/render_util.hpp"

int mouseX;
int mouseY;
int mouseScroll;

bool mousePressed;
bool mouseDown;
bool mousePrevDown;

SDL_Window* currentWindow;
int windowWidth;
int windowHeight;

SDL_Cursor* next_cursor = nullptr;

std::vector<SDL_Keycode> heldKeys = {};
std::vector<SDL_Keycode> pressedKeys = {};

bool ffmpegInstalled = true;

bool is_key_pressed(SDL_Keycode code) {
    return std::find(pressedKeys.begin(), pressedKeys.end(), code) != pressedKeys.end();
}

bool is_key_held(SDL_Keycode code) {
    return std::find(heldKeys.begin(), heldKeys.end(), code) != heldKeys.end();
}

bool update() {
    next_cursor = cursor_default;
    mouseDown = SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON_LMASK;
    mousePressed = !mousePrevDown && mouseDown;
    mousePrevDown = mouseDown;
    SDL_GetWindowSize(currentWindow, &windowWidth, &windowHeight);
    SDL_Event event;
    mouseScroll = 0;
    pressedKeys.clear();
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) return true;
        if (event.type == SDL_MOUSEWHEEL) mouseScroll = -event.wheel.y;
        if (event.type == SDL_KEYDOWN) {
            SDL_Keycode keycode = event.key.keysym.sym;
            if (!is_key_held(keycode)) {
                heldKeys.push_back(keycode);
                pressedKeys.push_back(keycode);
            }
        }
        if (event.type == SDL_KEYUP) {
            SDL_Keycode keycode = event.key.keysym.sym;
            if (is_key_held(keycode)) heldKeys.erase(std::find(heldKeys.begin(), heldKeys.end(), keycode));
        }
    }
    return false;
}

void render(SDL_Renderer* renderer) {
    render_gui(renderer);
}

bool render_ffmpeg_error(SDL_Renderer* renderer) {
    bool hovered = mouseX >= 102 && mouseY >= 46 && mouseX < 102 + 64 && mouseY < 46 + 16;
    if (hovered && mousePressed) return true;
    render_rect(renderer, 0, 0, 171, 67, 0x202020FF);
    render_text(renderer, 5, 5, "FFmpeg is not installed");
    render_text(renderer, 5, 24, "Please install FFmpeg");
    render_rect(renderer, 102, 46, 64, 16, hovered ? 0x404040FF : 0x303030FF);
    render_text(renderer, 127, 48, "OK");
    return false;
}

bool check_ffmpeg() {
    FILE* cmd = popen("ffmpeg" NULLFILE, "r");
    int exit = WEXITSTATUS(pclose(cmd));
    return exit == 1;
}

int main(int argc, char** argv) {
    if (!check_ffmpeg()) ffmpegInstalled = false;
    SDL_Window* window = SDL_CreateWindow("Titan Video Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ffmpegInstalled ? 1280 : 171, ffmpegInstalled ? 720 : 67, 0);
    SDL_SetWindowResizable(window, ffmpegInstalled ? SDL_TRUE : SDL_FALSE);
    currentWindow = window;
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags);
    init_cursors();
    init_icons(renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    while (true) {
        clock_t before = clock();
        if (update()) break;
        if (ffmpegInstalled) render(renderer);
        else if (render_ffmpeg_error(renderer)) break;
        SDL_SetCursor(next_cursor);
        SDL_RenderPresent(renderer);
        clock_t after = clock();
        if (after - before < CLOCKS_PER_SEC / 60) std::this_thread::sleep_for(std::chrono::microseconds((int)(1000000 / 60.0f - (float)(after - before) / CLOCKS_PER_SEC * 1000000)));
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}