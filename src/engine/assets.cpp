#include <engine/pch.h>

#include <engine/assets.h>

namespace cgt
{

std::vector<u8> LoadFile(const char* path)
{
    auto* fileRW = SDL_RWFromFile(path, "rb");
    CGT_ASSERT_ALWAYS_MSG(fileRW != nullptr, "Failed to open a file at: {}\nSDL Error: {}", path, SDL_GetError());

    usize fileSize = 0;
    void* fileData = SDL_LoadFile_RW(fileRW, &fileSize, true);
    CGT_ASSERT_ALWAYS_MSG(fileData != nullptr, "Failed to load a file at: {}\nSDL Error: {}", path, SDL_GetError());

    std::vector<u8> data(fileSize);
    memcpy(data.data(), fileData, fileSize);
    SDL_free(fileData);

    return data;
}

}