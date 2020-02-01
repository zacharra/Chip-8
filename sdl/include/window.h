#pragma once

#include <chip8/chip8.h>
#include "sdlpp.h"

class Window
{
public:
    Window(Chip8Cpu& chip8, int width, int height);
    ~Window();
    Window(const Window&) = delete;
    void operator =(const Window&) = delete;

    void run();

private:
    Chip8Cpu& m_chip8;
    sdl::Window m_window;
    sdl::Renderer m_renderer;
    sdl::Texture m_canvas;

    void render();

    void key_press(int key);
    void key_release(int key);

    bool m_done{};
};
