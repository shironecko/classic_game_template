#include <engine/pch.h>

extern int GameMain();

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    auto result = IMG_Init(IMG_INIT_PNG);
    CGT_ASSERT_ALWAYS_MSG(result & IMG_INIT_PNG, "Failed to initialize SDL2 Image library!");

    int gameReturnCode = GameMain();

    IMG_Quit();
    SDL_Quit();

    return gameReturnCode;
}