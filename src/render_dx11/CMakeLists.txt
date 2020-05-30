add_library(render_dx11
    render_context_dx11.cpp render_context_dx11.h
    pch.h system.h)

target_link_libraries(render_dx11
    PUBLIC
        engine
        render_core
    PRIVATE
        d3d11
        d3dcompiler
        dxgi)

target_precompile_headers(render_dx11 PRIVATE pch.h)