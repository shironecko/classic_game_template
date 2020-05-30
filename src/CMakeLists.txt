add_subdirectory(engine)
add_subdirectory(examples)
add_subdirectory(render_core)

IF (WIN32)
    add_subdirectory(render_dx11)
ENDIF ()
