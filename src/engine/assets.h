#pragma once

namespace cgt
{

const std::filesystem::path& GetGameRoot();
const std::filesystem::path& GetAssetsRoot();

std::filesystem::path AssetPath(const std::filesystem::path& relativePath);

std::vector<u8> LoadFileBytes(const std::filesystem::path& absolutePath);

}
