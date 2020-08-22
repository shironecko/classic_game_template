#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/closest_point.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <bindings/imgui_impl_sdl.h>

#include <engine/extern/tileson.hpp>
#include <engine/extern/tracy/Tracy.hpp>
#include <engine/extern/im3d/im3d.h>
#include <engine/extern/metrics_gui/metrics_gui/include/metrics_gui/metrics_gui.h>
