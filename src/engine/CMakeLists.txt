add_subdirectory(render)

add_library(engine
    engine_main.cpp
    window.cpp window.h
    pch.cpp pch.h external_libs.h
    system.cpp system.h
    assets.cpp assets.h
    clock.cpp clock.h
    ui_bindings.cpp ui_bindings.h
    math.cpp math.h
    tileset_helper.cpp tileset_helper.h
    engine.cpp engine.h
    input.cpp input.h
    audio.cpp audio.h
    key_codes.h
    ui_extensions.cpp ui_extensions.h
    api.h
    extern/tracy/TracyClient.cpp
    extern/im3d/im3d.cpp
    extern/metrics_gui/metrics_gui/source/metrics_gui.cpp)

target_compile_definitions(engine
    PUBLIC
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_FORCE_LEFT_HANDED
    TRACY_ENABLE
    _WINSOCKAPI_
    NOMINMAX)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2-Mixer CONFIG REQUIRED)
find_package(SDL2-Image CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(engine
    PUBLIC
        SDL2::SDL2 SDL2::SDL2main SDL2::SDL2_mixer SDL2::SDL2_image
        fmt::fmt fmt::fmt-header-only
        imgui::imgui
        glm
        render_core
)

target_precompile_headers(engine PRIVATE pch.h)

target_include_directories(engine
    PUBLIC
        ../)