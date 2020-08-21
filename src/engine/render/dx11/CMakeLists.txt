add_library(render_backend
    render_context_dx11.cpp render_context_dx11.h
    im3d_dx11.cpp im3d_dx11.h
    util.cpp util.h
    pch.cpp pch.h
    system.h)

find_library(DIRECTXTK_LIBRARY DirectXTK)
find_path(DIRECTXTK_INCLUDE DirectXTK/CommonStates.h)

target_link_libraries(render_backend
    PUBLIC
        engine
        render_core
    PRIVATE
        d3d11
        d3dcompiler
        dxgi
        ${DIRECTXTK_LIBRARY})

target_include_directories(render_backend PRIVATE ${DIRECTXTK_INCLUDE})

target_precompile_headers(render_backend PRIVATE pch.h)