#pragma once

namespace cgt
{

const std::filesystem::path& GetGameRoot();
const std::filesystem::path& GetAssetsRoot();

std::vector<u8> LoadFileBytes(const std::filesystem::path& absolutePath);
std::vector<u8> LoadAssetBytes(const std::filesystem::path& relativePath);

tmx_map* LoadTiledMap(const std::filesystem::path& relativePath);

}
