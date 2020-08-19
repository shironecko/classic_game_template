add_library(render_core
    i_render_context.h
    sprite_draw_list.cpp sprite_draw_list.h
    missingno.png.h
    i_camera.h
    camera_simple_ortho.cpp camera_simple_ortho.h
    api.h)

target_link_libraries(render_core
    PUBLIC
        engine)

target_precompile_headers(render_core PRIVATE pch.h)