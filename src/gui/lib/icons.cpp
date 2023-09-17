#include "icons.h"
#include "images.h"

// This file is automatically generated by ImageToC
// Do not modify!

SDL_Texture* icon_file = nullptr;
SDL_Texture* icon_folder = nullptr;
SDL_Texture* icon_up_dir = nullptr;
SDL_Texture* icon_font = nullptr;

SDL_Texture* create_texture(SDL_Renderer* renderer, unsigned int* data, int width, int height) {
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(data, width, height, 32, width * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    return SDL_CreateTextureFromSurface(renderer, surface);
}

void init_icons(SDL_Renderer* renderer) {
   icon_file = create_texture(renderer, icon_file_data, 16, 16);
   icon_folder = create_texture(renderer, icon_folder_data, 16, 16);
   icon_up_dir = create_texture(renderer, icon_up_dir_data, 16, 16);
   icon_font = create_texture(renderer, icon_font_data, 112, 84);
}