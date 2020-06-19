#pragma once

namespace cgt::render
{

ComPtr<ID3D10Blob>
CompileShader(const std::filesystem::path& absolutePath, const char* entryPoint, const char* profile, const D3D_SHADER_MACRO* defines);

ComPtr<ID3D11Buffer>
CreateBuffer(ID3D11Device* device, const void* data, usize size, UINT bindFlags, D3D11_USAGE usage, u32 cpuFlags);

void UpdateBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const void* data, u32 size);

template<typename T>
inline void UpdateBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const T& data)
{
    UpdateBuffer(context, buffer, &data, sizeof(data));
}

inline ComPtr<ID3D11Buffer> CreateStaticBuffer(ID3D11Device* device, const void* data, usize size, UINT bindFlags)
{
    CGT_ASSERT(data);
    return CreateBuffer(device, data, size, bindFlags, D3D11_USAGE_IMMUTABLE, 0);
}

template<typename T>
inline ComPtr<ID3D11Buffer> CreateStaticBuffer(ID3D11Device* device, const std::vector<T>& data, UINT bindFlags)
{
    return CreateStaticBuffer(device, data.data(), data.size() * sizeof(data[0]), bindFlags);
}

}
