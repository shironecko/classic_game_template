add_library(render_core
    i_render_context.h
    sprite_draw_list.cpp sprite_draw_list.h
    missingno.png.h
    camera.cpp camera.h
    api.h)

target_link_libraries(render_core
    PUBLIC
        engine
    PRIVATE
        render_backend)

target_precompile_headers(render_core PRIVATE pch.h)