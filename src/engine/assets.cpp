#include <engine/pch.h>

#include <engine/assets.h>

namespace cgt
{

const std::filesystem::path& GetGameRoot()
{
    auto FindGameRoot = []()
    {
        auto root = std::filesystem::current_path();
        bool gameRootFound = false;
        const usize MAX_DEPTH = 10;
        for (usize i = 0; i < MAX_DEPTH; ++i)
        {
            auto assetsPath = root / "assets";
            if (std::filesystem::exists(assetsPath))
            {
                gameRootFound = true;
                break;
            }

            root /= "..";
        }

        if (!gameRootFound)
        {
            auto currentPath = std::filesystem::current_path();
            CGT_PANIC("Failed to find the game's root folder! Launched from: {}", currentPath);
        }

        return std::filesystem::canonical(root);
    };

    static std::filesystem::path root = FindGameRoot();

    return root;
}

const std::filesystem::path& GetAssetsRoot()
{
    static std::filesystem::path assetsRoot = GetGameRoot() / "assets";

    return assetsRoot;
}

std::filesystem::path AssetPath(const std::filesystem::path& relativePath)
{
    CGT_ASSERT(relativePath.is_relative());

    return GetAssetsRoot() / relativePath;
}

std::vector<u8> LoadFileBytes(const std::filesystem::path& absolutePath)
{
    CGT_ASSERT(absolutePath.is_absolute());

    std::ifstream stream(absolutePath, std::ios::in | std::ios::binary);
    CGT_ASSERT_ALWAYS_MSG(stream.is_open(), "Failed to open a file at: {}\n", absolutePath);

    stream.seekg(0, std::ios::end);
    usize fileSize = stream.tellg();
    stream.seekg(0, std::ios::beg);

    std::vector<u8> data(fileSize);
    stream.read((char*)data.data(), fileSize);

    return data;

}

}