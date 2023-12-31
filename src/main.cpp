#include "main.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include<string>

#include "gui/gui_layout.hpp"
#include "gui/lib/cursors.hpp"
#include "gui/lib/icons.hpp"
#include "gui/render_util.hpp"
#include "utils.hpp"
//#include"../devs/project.hpp"


#include "libs/portable-file-dialogs.hpp"

namespace fs = std::filesystem;

int mouseX;
int mouseY;
int mouseScroll;

bool mousePressed;
bool mouseDown;
bool mousePrevDown;
bool rightMousePressed;
bool rightMouseDown;
bool rightMousePrevDown;

SDL_Window* currentWindow;
int windowWidth;
int windowHeight;

SDL_Cursor* next_cursor = nullptr;

std::vector<SDL_Keycode> heldKeys = {};
std::vector<SDL_Keycode> pressedKeys = {};

std::string tooltip = "";

SDL_Texture* buttonTexture =nullptr;
SDL_Rect buttonRect;

bool is_key_pressed(SDL_Keycode code) {
    return std::find(pressedKeys.begin(), pressedKeys.end(), code) != pressedKeys.end();
}

bool is_key_held(SDL_Keycode code) {
    return std::find(heldKeys.begin(), heldKeys.end(), code) != heldKeys.end();
}

bool update() {
    tooltip = "";
    next_cursor = cursor_default;
    int mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    mouseDown = mouseState & SDL_BUTTON_LMASK;
    mousePressed = !mousePrevDown && mouseDown;
    mousePrevDown = mouseDown;
    rightMouseDown = mouseState & SDL_BUTTON_RMASK;
    rightMousePressed = !rightMousePrevDown && rightMouseDown;
    rightMousePrevDown = rightMouseDown;
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
    if (tooltip != "") {
        int x = 8;
        int y = 8;
        int w = 8 + tooltip.length() * 7;
        int h = 8 + 14;
        if (mouseX > windowWidth - x - w - 8) x = x * -1 - w;
        if (mouseY > windowHeight - y - h - 8) y = y * -1 - h;
        render_rect(renderer, mouseX + x, mouseY + y, w, h, 0x0000007F);
        render_text(renderer, mouseX + x + 4, mouseY + y + 4, tooltip);
    }
}

bool check_ffmpeg() {
    std::string path = std::string(getenv("PATH"));
#ifdef WINDOWS
    char delimiter = ';';
    std::string suffix = ".exe";
#else
    char delimiter = ':';
    std::string suffix = "";
#endif
    std::vector<std::string> paths = split_string(delimiter, path);
    std::vector<std::string> dependencies = { "ffmpeg", "ffprobe", "ffplay" };
    for (std::string p : paths) {
        for (int i = 0; i < dependencies.size(); i++) {
            std::filesystem::path fsPath = std::filesystem::path(p) / (dependencies[i] + suffix);
            if (std::filesystem::exists(fsPath)) {
                dependencies.erase(dependencies.begin() + i);
                i--;
            }
        }
    }
    if (dependencies.size() == 0) return false;
    std::string missing = "";
    for (int i = 0; i < dependencies.size(); i++) {
        if (i != 0) missing += ", ";
        missing += dependencies[i];
    }
    pfd::message("Titan Video Editor", "Cannot find FFmpeg executables in PATH\n\nDependencies missing:\n" + missing, pfd::choice::ok, pfd::icon::error);
    return true;
}

std::string inputfile()
{
    FILE* pipe = popen("zenity --entry --text=\"name your project\"", "r");
    if (!pipe) {
        std::cerr << "Error opening pipe." << std::endl;
        //return EXIT_FAILURE;
    }

    // 取得zenity的輸出
    char buffer[128];
    std::ostringstream result_stream;
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result_stream << buffer;
    }

    // 關閉zenity行程
    pclose(pipe);

    // 取得user輸入
    std::string result = result_stream.str();

    // 印出user輸入
    std::cout << "您輸入的内容是: " << result;
    return result;
}


std::string openfile() 
{
    FILE* pipe = popen("zenity --file-selection --title=\"select your project\"", "r");
    if (!pipe) {
        std::cerr << "Failed to open the pipe." << std::endl;
        return NULL;
    }

    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) 
    {
        result += buffer;
    }

    std::cout << result << "\n";

    // 關閉zenity行程
    pclose(pipe);

    // 去除字串尾部的換行符號
     result.erase(std::remove_if(result.begin(), result.end(), [](char c) { return c == '\n'; }), result.end());

    // 使用filesystem取得資料夾路徑
    fs::path filePath(result);
    std::string folderPath = filePath.parent_path().string();
    // 印出user選擇的文件路徑
    std::cout << "您選擇的文件路徑是: " << folderPath << std::endl;
    return folderPath;
}


int selectproject()
{
    std::string command = "zenity --list --text=\"start\" --radiolist --column=\"\" --column=\"choice\" TRUE \"open a project\" FALSE \"create a new project\"";

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "無法執行 Zenity 命令\n";
        return EXIT_FAILURE;
    }

    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) 
    {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);


    if (result == "open a project\n") {
        std::cout << "open a project\n";
        return SELECTPROJECT;
    } else if (result == "create a new project\n") {
        std::cout << "create a new project\n";
        return NEWPROJECT;
    } 
}

void createDirectory(const std::string& projectpath,std::string& projectname) 
{
    std::string fullPath = projectpath + "/" + projectname;
    try 
    {
        // 嘗試創建路徑
        fs::create_directory(fullPath);
        std::cout << "創建路徑成功: " << fullPath << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "創建路徑失敗: " << e.what() << std::endl;
    }
}


int main(int argc, char** argv) {
    
    // SDL_Window* win=nullptr;
    // if(SDL_Init(SDL_INIT_VIDEO) < 0){
    //     std::cout << "SDL could not be initialized: " <<SDL_GetError();
    // }else
    // std::cout << "SDL video system is ready to go\n";

    // win = SDL_CreateWindow("C++ SDL2 Window",
    //         0,
    //         2500,
    //         640,
    //         480,
    //         SDL_WINDOW_SHOWN);
    // bool quit = false;
    // SDL_Event e;
    // while (!quit) {
    //     // Handle events on queue
    //     while (SDL_PollEvent(&e) != 0) {
    //         if (e.type == SDL_QUIT) {
    //             quit = true;
    //         }
    //     }

    //     SDL_Delay(16); // Adjust the delay based on your desired frame rate
    // }

    // SDL_DestroyWindow(win);
    // SDL_Quit();
    int aa=selectproject();
    std::string project_name;

    if(true)
    {
        switch(aa)
        {
        case SELECTPROJECT:
            project_path=openfile();
            std::cout<<"main "<<project_path<<"\n";
            break;
        case NEWPROJECT:
            project_path=openfile();
            std::cout<<"hell \n";
            project_name=inputfile();
            createDirectory(project_path,project_name);
            //buildproject(,project_path);
            break;

        default:
            printf("hi \n");
            break;
        }
    }

    
    
    // std::string i=inputfile();

    //openfile();

    
    
    
    
    
    
    
    
    if (check_ffmpeg()) return 1;
    SDL_Window* window = SDL_CreateWindow("Titan Video Editor - Alpha", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
    SDL_SetWindowResizable(window, SDL_TRUE);
    
    currentWindow = window;
    
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags);
    init_cursors();
    init_icons(renderer);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    
    while (true) {
        clock_t before = clock();
        if (update()) break;
        render(renderer);
        SDL_SetCursor(next_cursor);
        SDL_RenderPresent(renderer);
        SDL_RenderCopy(renderer,buttonTexture,NULL,&buttonRect);
        clock_t after = clock();
        if (after - before < CLOCKS_PER_SEC / 60) std::this_thread::sleep_for(std::chrono::microseconds((int)(1000000 / 60.0f - (float)(after - before) / CLOCKS_PER_SEC * 1000000)));
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(buttonTexture);
    SDL_Quit();
    return 0;
}