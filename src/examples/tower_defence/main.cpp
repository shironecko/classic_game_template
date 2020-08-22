#include <examples/tower_defence/pch.h>

#include <examples/tower_defence/game.h>

int GameMain()
{
    cgt::RunGame<Game>("Tower Defence", { 1920, 1080 });

    return 0;
}