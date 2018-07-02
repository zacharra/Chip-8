#pragma once

#include <SDL_timer.h>

class StopWatch
{
public:
    Uint32 elapsed_ms() const
    {
        return get_ticks() - m_current_ticks;
    }

    void update()
    {
        m_prev_ticks = m_current_ticks;
        m_current_ticks = get_ticks();
    }

    Uint32 current_ticks() const
    {
        return m_current_ticks;
    }

    static Uint32 get_ticks()
    {
        return SDL_GetTicks();
    }

private:
    Uint32 m_prev_ticks = 0;
    Uint32 m_current_ticks = 0;
};
