add_library(render_dx11
    render.cpp render.h
    pch.h system.h)

target_link_libraries(render_dx11
    PUBLIC
        engine
        render_core
    PRIVATE
        d3d11
        d3dcompiler)

target_precompile_headers(render_dx11 PRIVATE pch.h)