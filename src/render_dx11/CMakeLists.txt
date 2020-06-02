add_library(render_dx11
    render_context_dx11.cpp render_context_dx11.h
    util.cpp util.h
    pch.h system.h)

find_library(DIRECTXTK_LIBRARY DirectXTK)

target_link_libraries(render_dx11
    PUBLIC
        engine
        render_core
    PRIVATE
        d3d11
        d3dcompiler
        dxgi
        ${DIRECTXTK_LIBRARY})

target_precompile_headers(render_dx11 PRIVATE pch.h)