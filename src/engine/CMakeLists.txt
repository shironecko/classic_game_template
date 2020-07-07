add_library(engine
    engine_main.cpp
    window.cpp window.h
    pch.cpp pch.h external_libs.h system.h
    assets.cpp assets.h
    clock.cpp clock.h
    imgui_helper.cpp imgui_helper.h
    math.cpp math.h
    api.h
    extern/tracy/TracyClient.cpp
    extern/im3d/im3d.cpp tileset_helper.cpp tileset_helper.h)

target_compile_definitions(engine
    PUBLIC
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    GLM_FORCE_LEFT_HANDED
    TRACY_ENABLE
    _WINSOCKAPI_
    NOMINMAX)

find_package(SDL2 CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(engine
    PUBLIC
        SDL2::SDL2 SDL2::SDL2main
        fmt::fmt fmt::fmt-header-only
        imgui::imgui
        glm
        render_core
    PRIVATE
        $<$<PLATFORM_ID:Windows>:render_dx11>
)

target_precompile_headers(engine PRIVATE pch.h)

target_include_directories(engine
    PUBLIC
        ../)