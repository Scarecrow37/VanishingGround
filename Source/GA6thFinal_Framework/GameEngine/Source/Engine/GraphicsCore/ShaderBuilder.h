#pragma once

class Shader;
class ShaderBuilder
{
	enum State { None, Begin, End };
public:
    ShaderBuilder();
    ~ShaderBuilder() = default;

public:
	enum class Type { VS, HS, DS, GS, PS, CS, MS, AS, END };

public:
	ComPtr<ID3D12RootSignature> GetRootSignature() const { return _rootSignature; }
    const D3D12_SHADER_BYTECODE& GetShaderByteCode(ShaderBuilder::Type type) const { return _shaderByteCodes[(int)type]; }
	const D3D12_INPUT_LAYOUT_DESC& GetInputLayout() const { return _inputLayout; }
	UINT GetRootSignatureIndex(std::string_view tag) const;

public:
	void BeginBuild();
	void EndBuild();
    HRESULT SetShader(std::wstring_view filePath, ShaderBuilder::Type type);

private:
	
	HRESULT CreateRootSignature();
	HRESULT CreateRootSignature_ver0();
	void CreateInputLayout(ComPtr<ID3D12ShaderReflection> shaderReflection, const D3D12_SHADER_DESC& shaderDesc);
	void CreateStaticSampler(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addressMode, UINT shaderRegister, D3D12_STATIC_SAMPLER_DESC& desc);
	D3D12_STATIC_SAMPLER_DESC FindStaticSampler(std::string_view tag);

private:
	using StaticSamplers = std::unordered_map<std::string, D3D12_STATIC_SAMPLER_DESC>;
	static StaticSamplers	_staticSamplers;
	static bool				_isFirstInitialize;

private:
	std::unordered_map<std::string, UINT> 		_rootSignatureIndex;
    std::vector<std::shared_ptr<Shader>>        _shaders;
	std::vector<D3D12_SHADER_BYTECODE>			_shaderByteCodes;
	std::vector<std::string>					_savedSementicNames;
	std::vector<D3D12_INPUT_ELEMENT_DESC>		_inputElements;
	D3D12_INPUT_LAYOUT_DESC						_inputLayout;
	ComPtr<ID3D12RootSignature>					_rootSignature;
    ShaderBuilder::State                        _currentState;
};