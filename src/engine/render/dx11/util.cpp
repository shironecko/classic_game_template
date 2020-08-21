#include <engine/render/dx11/pch.h>

#include <engine/render/dx11/util.h>
#include <engine/assets.h>

namespace cgt::render
{

ComPtr<ID3D10Blob>
CompileShader(const std::filesystem::path& absolutePath, const char* entryPoint, const char* profile, const D3D_SHADER_MACRO* defines)
{
    CGT_ASSERT(entryPoint && profile);

    auto shaderData = LoadFileBytes(absolutePath);

    std::string utf8Path = absolutePath.u8string();
    ComPtr<ID3D10Blob> shader;
    ComPtr<ID3D10Blob> errors;
    HRESULT hresult = D3DCompile(
        shaderData.data(),
        shaderData.size(),
        utf8Path.c_str(),
        defines,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint,
        profile,
        0,
        0,
        shader.GetAddressOf(),
        errors.GetAddressOf());

    if (!SUCCEEDED(hresult))
    {
        const char* err = (const char*)errors->GetBufferPointer();
        CGT_PANIC("Failed to compile a shader at: {}!\nErrors:\n{}", absolutePath, err);
    }

    return shader;
}

ComPtr<ID3D11Buffer>
CreateBuffer(ID3D11Device* device, const void* data, usize size, UINT bindFlags, D3D11_USAGE usage, u32 cpuFlags)
{
    CGT_ASSERT(size);

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = size;
    bufferDesc.BindFlags = bindFlags;
    bufferDesc.Usage = usage;
    bufferDesc.CPUAccessFlags = cpuFlags;

    D3D11_SUBRESOURCE_DATA subresData = {};
    subresData.pSysMem = data;

    ComPtr<ID3D11Buffer> buffer;
    HRESULT hresult = device->CreateBuffer(
        &bufferDesc,
        data ? &subresData : nullptr,
        buffer.GetAddressOf());

    CGT_CHECK_HRESULT(hresult, "Buffer creation failed!");

    return buffer;
}

void UpdateBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const void* data, u32 size)
{
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, data, size);
    context->Unmap(buffer, 0);
}

}
