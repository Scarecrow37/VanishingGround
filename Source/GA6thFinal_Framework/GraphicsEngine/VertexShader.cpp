#include "pch.h"
#include "VertexShader.h"

VertexShader::VertexShader()
{
    _type = ShaderType::VERTEX;
}

void VertexShader::LoadResource(const std::filesystem::path& filePath, const std::function<void()>& callback)
{
    CompileShader(filePath.c_str(), "vs_main", "vs_5_1");
}

void VertexShader::CreateInputLayout(ComPtr<ID3D12ShaderReflection> shaderReflection,
                                     const D3D12_SHADER_DESC&       shaderDesc)
{
    if (_isInputLayoutCreated)
    {
        return;
    }

	_inputElements.reserve(shaderDesc.InputParameters);
	_savedSemanticNames.resize(shaderDesc.InputParameters);

	for (UINT i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
		shaderReflection->GetInputParameterDesc(i, &paramDesc);

		// DXGI 포맷 결정 (Semantic Name에 따라 자동 결정)
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		UINT componentCount = 0;

		// Mask 값으로 데이터 크기 판별
		if		(paramDesc.Mask == 1)	componentCount = 1;  // R
		else if (paramDesc.Mask <= 3)	componentCount = 2;  // RG
		else if (paramDesc.Mask <= 7)	componentCount = 3;  // RGB
		else if (paramDesc.Mask <= 15)	componentCount = 4; // RGBA

		// 32비트 타입 판별
		if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
		{
			if		(componentCount == 1) format = DXGI_FORMAT_R32_UINT;
			else if (componentCount == 2) format = DXGI_FORMAT_R32G32_UINT;
			else if (componentCount == 3) format = DXGI_FORMAT_R32G32B32_UINT;
			else if (componentCount == 4) format = DXGI_FORMAT_R32G32B32A32_UINT;
		}
		else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
		{
			if		(componentCount == 1) format = DXGI_FORMAT_R32_SINT;
			else if (componentCount == 2) format = DXGI_FORMAT_R32G32_SINT;
			else if (componentCount == 3) format = DXGI_FORMAT_R32G32B32_SINT;
			else if (componentCount == 4) format = DXGI_FORMAT_R32G32B32A32_SINT;
		}
		else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
		{
			if		(componentCount == 1) format = DXGI_FORMAT_R32_FLOAT;
			else if (componentCount == 2) format = DXGI_FORMAT_R32G32_FLOAT;
			else if (componentCount == 3) format = DXGI_FORMAT_R32G32B32_FLOAT;
			else if (componentCount == 4) format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		// Input Layout 구조체 생성
		_savedSemanticNames[i].resize(strlen(paramDesc.SemanticName) + 1);
		memcpy(_savedSemanticNames[i].data(), paramDesc.SemanticName, strlen(paramDesc.SemanticName) + 1);

		D3D12_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName = _savedSemanticNames[i].data();
		inputElement.SemanticIndex = paramDesc.SemanticIndex;
		inputElement.Format = format;
		inputElement.InputSlot = 0;
		inputElement.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate = 0;

		// Input Layout에 추가
		_inputElements.push_back(inputElement);
	}

	_inputLayout.NumElements        = shaderDesc.InputParameters;
    _inputLayout.pInputElementDescs = _inputElements.data();
    _isInputLayoutCreated           = true;
}