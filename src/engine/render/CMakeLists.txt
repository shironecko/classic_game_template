add_subdirectory(core)

if (WIN32)
    add_subdirectory(dx11)
else()
    message(Platform is not supported!)
endif()