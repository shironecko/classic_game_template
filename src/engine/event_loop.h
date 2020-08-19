#pragma once

#include "window.h"

namespace cgt
{

class Window;

class IEventListener
{
public:
    enum class EventAction
    {
        Consume,
        Passthrough,
    };

    virtual EventAction OnEvent(const SDL_Event& event) = 0;

    virtual ~IEventListener() = default;
};

class EventLoop
{
public:
    explicit EventLoop(std::shared_ptr<Window> window);

    void AddEventListener(std::shared_ptr<IEventListener> listener);
    bool PollEvent(SDL_Event& outEvent);

private:
    std::shared_ptr<Window> m_Window;
    std::vector<std::shared_ptr<IEventListener>> m_EventListeners;
};

}
