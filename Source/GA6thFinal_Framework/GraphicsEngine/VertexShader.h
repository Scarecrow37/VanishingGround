#pragma once
#include "Shader.h"

class VertexShader : public Shader
{
public:
    VertexShader();
    virtual ~VertexShader() = default;

public:
    const D3D12_INPUT_LAYOUT_DESC& GetInputLayout() const { return _inputLayout; }

public:
    void CreateInputLayout(ComPtr<ID3D12ShaderReflection> shaderReflection, const D3D12_SHADER_DESC& shaderDesc);

public:
    // Shader을(를) 통해 상속됨
    void LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback = nullptr) override;

private:    
    std::vector<std::string>              _savedSemanticNames;
    std::vector<D3D12_INPUT_ELEMENT_DESC> _inputElements;
    D3D12_INPUT_LAYOUT_DESC               _inputLayout;
    bool                                  _isInputLayoutCreated = false;
};
