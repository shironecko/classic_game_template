#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/game.h>

int GameMain()
{
    cgt::Engine engine("Tower Defence", { 1920, 1080 });
    engine.Run<Game>();

    return 0;
}