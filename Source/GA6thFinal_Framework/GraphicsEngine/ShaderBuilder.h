#pragma once

class Shader;
class ShaderBuilder
{
    enum State { NONE, BEGINE, END };

public:
    ShaderBuilder();
    ~ShaderBuilder() = default;

public:
    enum class Type { VS, HS, DS, GS, PS, CS, MS, AS, END };
    enum class BindType { TABLE, DIRECT };

public:
    ID3D12RootSignature*           GetRootSignature() const { return _rootSignature.Get(); }
    const D3D12_SHADER_BYTECODE&   GetShaderByteCode(ShaderBuilder::Type type) const { return _shaderByteCodes[(int)type]; }
    const D3D12_INPUT_LAYOUT_DESC& GetInputLayout() const;
    UINT                           GetRootParameterIndex(std::string_view tag) const;

public:
    static void CreateStaticSampler(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addressMode, D3D12_COMPARISON_FUNC func, UINT shaderRegister, D3D12_STATIC_SAMPLER_DESC& desc);

public:
    void BeginBuild();
    void EndBuild(BindType type = BindType::DIRECT);
    void SetShader(std::wstring_view filePath, ShaderBuilder::Type type);

private:
    static std::unordered_map<std::string, D3D12_STATIC_SAMPLER_DESC>& GetStaticSamplers();

private:
    void CreateRootSignatureTable();
    void CreateRootSignatureDirect();

private:
    std::unordered_map<std::string, UINT> _rootParameterIndex;
    std::vector<std::shared_ptr<Shader>>  _shaders;
    std::vector<D3D12_SHADER_BYTECODE>    _shaderByteCodes;
    ComPtr<ID3D12RootSignature>           _rootSignature;
    ShaderBuilder::State                  _currentState;
};