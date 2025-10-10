#pragma once
#include "Shader.h"

class ComputeShader : public Shader
{
public:
    ComputeShader();
    virtual ~ComputeShader() = default;

public:
    // Shader을(를) 통해 상속됨
    void LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback = nullptr) override;
};
