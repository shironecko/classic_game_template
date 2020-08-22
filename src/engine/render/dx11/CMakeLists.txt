add_library(render_backend
    render_context_dx11.cpp render_context_dx11.h
    im3d_dx11.cpp im3d_dx11.h
    util.cpp util.h
    pch.cpp pch.h
    system.h)

target_link_libraries(render_backend
    PUBLIC
        engine
        render_core
    PRIVATE
        d3d11
        d3dcompiler
        dxgi
)

target_precompile_headers(render_backend PRIVATE pch.h)