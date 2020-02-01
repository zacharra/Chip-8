#pragma once

#include <SDL_timer.h>

class StopWatch
{
public:
    StopWatch() = default;

    Uint32 elapsed_ms() const
    {
        return get_ticks() - m_current_ticks;
    }

    void update()
    {
        m_current_ticks = get_ticks();
    }

    static Uint32 get_ticks()
    {
        return SDL_GetTicks();
    }

private:
    Uint32 m_current_ticks = get_ticks();
};
