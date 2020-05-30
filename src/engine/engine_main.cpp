#include <engine/pch.h>

extern int GameMain();

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);

    return GameMain();

    SDL_Quit();
}