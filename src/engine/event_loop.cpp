#include <engine/pch.h>

#include <engine/event_loop.h>
#include <engine/window.h>

namespace cgt
{

EventLoop::EventLoop(std::shared_ptr<Window> window)
    : m_Window(std::move(window))
{
}

void EventLoop::AddEventListener(std::shared_ptr<IEventListener> listener)
{
    m_EventListeners.emplace_back(std::move(listener));
}

bool EventLoop::PollEvent(SDL_Event& outEvent)
{
    SDL_Event event {};
    while (m_Window->PollEvent(event))
    {
        bool eventConsumed = false;
        for (auto& listener : m_EventListeners)
        {
            if (listener->OnEvent(event) == IEventListener::EventAction::Consume)
            {
                eventConsumed = true;
                break;
            }
        }

        if (!eventConsumed)
        {
            outEvent = event;
            return true;
        }
    }

    return false;
}

}