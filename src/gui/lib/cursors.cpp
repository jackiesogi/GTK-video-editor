#include "cursors.hpp"
#include "images.hpp"

// This file is automatically generated by ImageToC
// Do not modify!

SDL_Cursor* cursor_default = nullptr;
SDL_Cursor* cursor_move = nullptr;
SDL_Cursor* cursor_move_vertical = nullptr;
SDL_Cursor* cursor_move_horizontal = nullptr;
SDL_Cursor* cursor_clip_right = nullptr;
SDL_Cursor* cursor_clip_left = nullptr;

SDL_Surface* create_surface(unsigned int* data, int width, int height) {
    return SDL_CreateRGBSurfaceFrom(data, width, height, 32, width * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
}

void init_cursors() {
    cursor_default = SDL_GetDefaultCursor();
    cursor_move = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
    cursor_move_vertical = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
    cursor_move_horizontal = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
    cursor_clip_right = SDL_CreateColorCursor(create_surface(cursor_clip_right_data, 16, 16), 8, 8);
    cursor_clip_left = SDL_CreateColorCursor(create_surface(cursor_clip_left_data, 16, 16), 8, 8);
}