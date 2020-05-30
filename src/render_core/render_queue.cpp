#include <render_core/pch.h>

#include <render_core/render_queue.h>

namespace cgt::render
{

void RenderQueue::Reset()
{
    clearColor = glm::vec4(1.0f);
    sprites.clear();
}

}