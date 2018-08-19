#pragma once

#include <SDL.h>
#include <stdexcept>
#include <chip8/utils/resource_ptr.h>

/**
 * SDL helper functions for easy error handling et al
 * The call() guard functions have been inspired by this project:
 * https://github.com/antonte/sdlpp.git
 */
namespace sdl
{
    class Error: public std::runtime_error
    {
    public:
        Error()
            : std::runtime_error(SDL_GetError())
        {
            SDL_ClearError();
        }
    };

    template <class... Args, class... Args2>
    int call(int (f)(Args...), Args2... args)
    {
        int r = f(args...);
        if (r < 0) {
            throw Error();
        }

        return r;
    }

    template <class R, class... Args, class... Args2>
    R call(R (f)(Args...), Args2... args)
    {
        return f(args...);
    }

    template <typename R, class... Args, class... Args2>
    R* call(R *(f)(Args...), Args2... args)
    {
        R* r = f(args...);
        if (r == nullptr) {
            throw Error();
        }

        return r;
    }

    template <class... Args, class... Args2>
    void call(void (f)(Args...), Args2... args)
    {
        f(args...);
    }

    class Init
    {
    public:
        static Init init;
    private:
        template <class... Args>
        Init(Args... args)
        {
            call(SDL_Init, args...);
        }

        Init(const Init&) = delete;
        Init &operator=(const Init&) = delete;

        ~Init()
        {
            SDL_Quit();
        }
    };

    using Window = utils::resource_ptr<SDL_Window, SDL_DestroyWindow>;
    using Renderer = utils::resource_ptr<SDL_Renderer, SDL_DestroyRenderer>;
    using Texture = utils::resource_ptr<SDL_Texture, SDL_DestroyTexture>;
}
