#pragma once

namespace cgt
{

const char* GetGameRoot();
std::vector<u8> LoadFile(const char* path);
tmx_map* LoadTiledMap(const char* path);

}
