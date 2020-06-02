add_library(render_core
    render_config.cpp render_config.h
    i_render_context.h
    render_queue.cpp render_queue.h
    missingno.png.h i_camera.h camera_simple_ortho.cpp camera_simple_ortho.h)

target_link_libraries(render_core
    PUBLIC
        engine)

target_precompile_headers(render_core PRIVATE pch.h)