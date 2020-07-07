add_executable(tower_defence
    main.cpp
    game_state.cpp game_state.h
    map_data.cpp map_data.h
    enemy_data.cpp enemy_data.h
    tower_data.cpp tower_data.h
    pch.h projectile_data.cpp projectile_data.h)

target_link_libraries(tower_defence
    engine
)

target_precompile_headers(tower_defence PRIVATE pch.h)
