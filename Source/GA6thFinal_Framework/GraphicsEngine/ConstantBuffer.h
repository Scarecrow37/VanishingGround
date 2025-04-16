#pragma once
#include "Base.h"

class ConstantBuffer : public Base
{
    struct Data
    {
        ID3D11Buffer* pBuffer;
        unsigned int size;
    };
public:
    explicit ConstantBuffer();
    virtual ~ConstantBuffer() = default;

public:
    void AddConstantBuffer(const std::filesystem::path& name, const unsigned int byteWidth);
    void SetConstantBuffer(const wchar_t* name, ShaderType type, unsigned int start = 0, unsigned int numBuffers = 1);
    void UpdateConstantBuffer(const wchar_t* name, const void* pData);

private:
    // DXBase을(를) 통해 상속됨
    void Free() override;

private:
    std::unordered_map<std::wstring, Data>  _datas;
    ID3D11DeviceContext*                    _pDeviceContext = nullptr;
};