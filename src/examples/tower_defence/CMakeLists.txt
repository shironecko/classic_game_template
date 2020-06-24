add_executable(tower_defence
    main.cpp
    tilemap.cpp tilemap.h
    pch.h)

target_link_libraries(tower_defence
    engine
)

target_precompile_headers(tower_defence PRIVATE pch.h)
