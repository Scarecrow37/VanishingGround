#pragma once
#include "Shader.h"

class PixelShader : public Shader
{
public:
    PixelShader()           = default;
    virtual ~PixelShader() = default;

public:
    // Shader을(를) 통해 상속됨
    HRESULT LoadResource(const std::filesystem::path& filePath) override;
};
