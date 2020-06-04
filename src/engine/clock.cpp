#include <engine/pch.h>

#include <engine/clock.h>

namespace cgt
{

Clock::Clock() {
    m_Last = SDL_GetPerformanceCounter();
}

float Clock::Tick() {
    uint64_t now = SDL_GetPerformanceCounter();
    float delta = float((now - m_Last) / (double)SDL_GetPerformanceFrequency());
    m_Last = now;

    return delta;
}

}
