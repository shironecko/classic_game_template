#pragma once

template <>
struct ::std::default_delete<tmx_map> {
    default_delete() = default;
    void operator()(tmx_map* p) const noexcept { tmx_map_free(p); }
};

namespace cgt
{

const char* GetGameRoot();
std::vector<u8> LoadFile(const char* path);
std::unique_ptr<tmx_map> LoadTiledMap(const char* path);

}
