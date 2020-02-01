#include "window.h"

#include "stopwatch.h"

Window::Window(Chip8Cpu& chip8, int width, int height)
    : m_chip8(chip8)
{
    m_window = sdl::Window{sdl::call(SDL_CreateWindow, "Chip-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN)};
    m_renderer = sdl::Renderer{sdl::call(SDL_CreateRenderer, m_window.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)};
    m_canvas = sdl::Texture{sdl::call(SDL_CreateTexture, m_renderer.get(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, Chip8Cpu::screen_width, Chip8Cpu::screen_height)};
}

Window::~Window() = default;

void Window::run()
{
    m_done = false;
    m_chip8.reset();

    SDL_Event evt;
    StopWatch watch;

    while (!m_done) {
        while (SDL_PollEvent(&evt)) {
            switch (evt.type) {
            case SDL_QUIT:
                m_done = true;
                break;
            case SDL_KEYDOWN:
                switch (evt.key.keysym.sym) {
                case SDLK_q:
                    if (evt.key.keysym.mod & KMOD_LCTRL) {
                case SDLK_ESCAPE:
                        m_done = true;
                        break;
                    }
                case SDLK_RETURN:
                case SDLK_KP_ENTER:
                    if (evt.key.keysym.mod & KMOD_LALT) {
                case SDLK_F11:
                        auto flags = SDL_GetWindowFlags(m_window.get());
                        auto set_fs = flags & SDL_WINDOW_FULLSCREEN_DESKTOP ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP;
                        sdl::call(SDL_SetWindowFullscreen, m_window.get(), set_fs);
                        int w, h;
                        SDL_GetWindowSize(m_window.get(), &w, &h);
                        sdl::call(SDL_RenderSetLogicalSize, m_renderer.get(), w, h);
                        break;
                    }
                default:
                    key_press(evt.key.keysym.sym);
                }
                break;
            case SDL_KEYUP:
                key_release(evt.key.keysym.sym);
                break;
            default:
                ;
            }
        }

        m_chip8.step();

        if (m_chip8.flags.cls) {
            m_chip8.clear_screen();
            m_chip8.flags.cls = false;
        }

        if (m_chip8.flags.draw) {
            render();
            m_chip8.flags.draw = false;
        }

        if (watch.elapsed_ms() >= 1000/60) {
            m_chip8.count_down();
            watch.update();
        }
    }
}

void Window::render()
{
    sdl::call(SDL_SetRenderDrawColor, m_renderer.get(), 0, 0, 0, 255);
    sdl::call(SDL_RenderClear, m_renderer.get());

    void* pixels_;
    int pitch;
    sdl::call(SDL_LockTexture, m_canvas.get(), nullptr, &pixels_, &pitch);
    auto pixels = static_cast<Uint32*>(pixels_);
    for (int y = 0; y < Chip8Cpu::screen_height; y++) {
        for (int x = 0; x < Chip8Cpu::screen_width; x++) {
            pixels[y * Chip8Cpu::screen_width + x] = m_chip8.gfx[y][x] ? static_cast<Uint32>(-1) : 0;
        }
    }
    sdl::call(SDL_UnlockTexture, m_canvas.get());

    sdl::call(SDL_RenderCopy, m_renderer.get(), m_canvas.get(), nullptr, nullptr);
    sdl::call(SDL_RenderPresent, m_renderer.get());
}

void Window::key_press(int key)
{
    auto& keys = m_chip8.keys;
    switch (key) {
    case SDLK_1:
        keys[1] = 1;
        break;
    case SDLK_2:
        keys[2] = 1;
        break;
    case SDLK_3:
        keys[3] = 1;
        break;
    case SDLK_4:
        keys[0xC] = 1;
        break;
    case SDLK_q:
        keys[4] = 1;
        break;
    case SDLK_w:
        keys[5] = 1;
        break;
    case SDLK_e:
        keys[6] = 1;
        break;
    case SDLK_r:
        keys[0xD] = 1;
        break;
    case SDLK_a:
        keys[7] = 1;
        break;
    case SDLK_s:
        keys[8] = 1;
        break;
    case SDLK_d:
        keys[9] = 1;
        break;
    case SDLK_f:
        keys[0xE] = 1;
        break;
    case SDLK_y:
        keys[0xA] = 1;
        break;
    case SDLK_x:
        keys[0] = 1;
        break;
    case SDLK_c:
        keys[0xB] = 1;
        break;
    case SDLK_v:
        keys[0xF] = 1;
        break;
    default:
        ;
    }
}

void Window::key_release(int key)
{
    auto& keys = m_chip8.keys;
    switch (key) {
    case SDLK_1:
        keys[1] = 0;
        break;
    case SDLK_2:
        keys[2] = 0;
        break;
    case SDLK_3:
        keys[3] = 0;
        break;
    case SDLK_4:
        keys[0xC] = 0;
        break;
    case SDLK_q:
        keys[4] = 0;
        break;
    case SDLK_w:
        keys[5] = 0;
        break;
    case SDLK_e:
        keys[6] = 0;
        break;
    case SDLK_r:
        keys[0xD] = 0;
        break;
    case SDLK_a:
        keys[7] = 0;
        break;
    case SDLK_s:
        keys[8] = 0;
        break;
    case SDLK_d:
        keys[9] = 0;
        break;
    case SDLK_f:
        keys[0xE] = 0;
        break;
    case SDLK_y:
        keys[0xA] = 0;
        break;
    case SDLK_x:
        keys[0] = 0;
        break;
    case SDLK_c:
        keys[0xB] = 0;
        break;
    case SDLK_v:
        keys[0xF] = 0;
        break;
    default:
        ;
    }
}
