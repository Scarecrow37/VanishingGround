#include "pch.h"
#include "Shader.h"

bool Shader::_isFirstInitialize = false;
Shader::StaticSamplers	Shader::_staticSamplers = {};

Shader::Shader()
	: _currentState(None)
{	
	if (!_isFirstInitialize)
	{
		CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0, _staticSamplers["samLinear_wrap"]);

		_isFirstInitialize = true;
	}
}

UINT Shader::GetRootSignatureIndex(std::string_view tag) const
{
	auto iter = _rootSignatureIndex.find(tag.data());

	if (iter != _rootSignatureIndex.end())
		return iter->second;

	return -1;
}

void Shader::BeginBuild()
{
	_currentState = Begin;

	_shaders.resize(static_cast<UINT>(Type::END));
	_shaderByteCodes.resize(static_cast<UINT>(Type::END));
}

void Shader::EndBuild()
{
	if (Begin != _currentState)
		return;

	bool hasShader = false;

	for (auto& shader : _shaders)
	{
		if (shader)
		{
			hasShader = true;
			break;
		}
	}

	if (hasShader)
		CreateRootSignature();

	_currentState = End;
}

HRESULT Shader::LoadShader(std::wstring_view filePath, Shader::Type type)
{
	if (_shaders.empty())
		return E_FAIL;

	std::string_view entry;
	std::string_view shaderModel;

	switch (type)
	{
	case Shader::Type::VS:
		entry = "vs_main";
		shaderModel = "vs_5_1";
		break;
	case Shader::Type::PS:
		entry = "ps_main";
		shaderModel = "ps_5_1";
		break;
	case Shader::Type::GS:
		entry = "gs_main";
		shaderModel = "gs_5_1";
		break;
	case Shader::Type::CS:
		entry = "cs_main";
		shaderModel = "cs_5_1";
		break;
	}

	UINT typeID = static_cast<UINT>(type);

	HRESULT hr = CompileShader(filePath, entry, shaderModel, _shaders[typeID]);
	FAILED_CHECK_BREAK(hr);

	_shaderByteCodes[typeID].pShaderBytecode = _shaders[typeID]->GetBufferPointer();
	_shaderByteCodes[typeID].BytecodeLength = _shaders[typeID]->GetBufferSize();
	
	return S_OK;
}

HRESULT Shader::CreateRootSignature()
{
	HRESULT hr = S_OK;

	std::vector<D3D12_ROOT_PARAMETER> rootParameters;
	std::vector<D3D12_DESCRIPTOR_RANGE> resourceRanges;
	std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;

	resourceRanges.reserve(32);

	for (int i = 0; i < (int)Type::END; i++)
	{
		ComPtr<ID3D12ShaderReflection> shaderReflection;
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;

		switch ((Type)i)
		{
		case Type::VS:
			visibility = D3D12_SHADER_VISIBILITY_VERTEX;
			break;
		case Type::PS:
			visibility = D3D12_SHADER_VISIBILITY_PIXEL;
			break;
		case Type::CS:
			visibility = D3D12_SHADER_VISIBILITY_ALL;
			break;
		}

		if (nullptr == _shaders[i]) continue;

		hr = D3DReflect(_shaders[i]->GetBufferPointer(), _shaders[i]->GetBufferSize(), IID_PPV_ARGS(shaderReflection.GetAddressOf()));
		FAILED_CHECK_BREAK(hr);

		D3D12_SHADER_DESC shaderDesc;
		shaderReflection->GetDesc(&shaderDesc);

		if ((int)Type::VS == i)
		{
			CreateInputLayout(shaderReflection, shaderDesc);
		}

		// 바인딩된 리소스 개수만큼 반복 (CBV, SRV, UAV, Sampler 등)
		for (UINT j = 0; j < shaderDesc.BoundResources; j++)
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			shaderReflection->GetResourceBindingDesc(j, &bindDesc);			
			
			D3D12_DESCRIPTOR_RANGE descriptorRange = {};
			descriptorRange.NumDescriptors = 1;
			descriptorRange.BaseShaderRegister = bindDesc.BindPoint;
			descriptorRange.RegisterSpace = bindDesc.Space;
			descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			if (bindDesc.Type == D3D_SIT_CBUFFER) // 상수 버퍼 (CBV)
			{
				std::string_view name = bindDesc.Name;
				D3D12_ROOT_PARAMETER rootParam = {};
				if (name.find("bit32") != std::string::npos)
				{
					rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
					rootParam.Constants.Num32BitValues = 1;
					rootParam.Constants.ShaderRegister = bindDesc.BindPoint;
					rootParam.Constants.RegisterSpace = 0;
					rootParam.ShaderVisibility = visibility;
				}
				else
				{
					rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
					rootParam.Descriptor.ShaderRegister = bindDesc.BindPoint;
					rootParam.Descriptor.RegisterSpace = 0;
					rootParam.ShaderVisibility = visibility;
				}
				rootParameters.push_back(rootParam);
			}
			else if (bindDesc.Type == D3D_SIT_TEXTURE ||	// 텍스처 (SRV)
				bindDesc.Type == D3D_SIT_STRUCTURED ||	// Structured Buffer (SRV)
				bindDesc.Type == D3D_SIT_BYTEADDRESS)	// ByteAddressBuffer (SRV)
			{
				std::string_view name = bindDesc.Name;
				if (name.find("textures") != std::string::npos)
				{
					descriptorRange.NumDescriptors = -1;
				}

				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				resourceRanges.push_back(descriptorRange);

				D3D12_ROOT_PARAMETER rootParam = {};
				rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;				
				rootParam.DescriptorTable.NumDescriptorRanges = 1;
				rootParam.DescriptorTable.pDescriptorRanges = &resourceRanges.back();
				rootParam.ShaderVisibility = visibility;

				rootParameters.push_back(rootParam);
			}
			else if (bindDesc.Type == D3D_SIT_UAV_RWTYPED ||	// RWTexture (UAV)
				bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED ||	// RWStructuredBuffer (UAV)
				bindDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS)		// RWByteAddressBuffer (UAV)
			{
				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				resourceRanges.push_back(descriptorRange);

				D3D12_ROOT_PARAMETER rootParam = {};
				rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				rootParam.DescriptorTable.NumDescriptorRanges = 1;
				rootParam.DescriptorTable.pDescriptorRanges = &resourceRanges.back();
				rootParam.ShaderVisibility = visibility;
				rootParameters.push_back(rootParam);
			}
			else if (bindDesc.Type == D3D_SIT_SAMPLER)
			{
				samplers.push_back(_staticSamplers[bindDesc.Name]);
			}

			auto iter = _rootSignatureIndex.find(bindDesc.Name);
			if (iter != _rootSignatureIndex.end())
			{
				rootParameters[iter->second].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
				rootParameters.pop_back();
			}
			else
			{
				_rootSignatureIndex[bindDesc.Name] = static_cast<UINT>(rootParameters.size() - 1);
			}
		}
	}

	// Root Signature 생성
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.NumParameters = static_cast<UINT>(rootParameters.size());
	rootSigDesc.pParameters = rootParameters.data();
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSigDesc.NumStaticSamplers = static_cast<UINT>(samplers.size());
	rootSigDesc.pStaticSamplers = samplers.data();

	// Root Signature 직렬화
	ComPtr<ID3DBlob> serializedRootSignature;
	ComPtr<ID3DBlob> errorBlob;
	hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSignature.GetAddressOf(), errorBlob.GetAddressOf());
	//FAILED_CHECK_BREAK(hr);

	if (nullptr != errorBlob)
	{
		std::filesystem::path errorMessage = static_cast<const char*>(errorBlob->GetBufferPointer());
		ASSERT(FAILED(hr), errorMessage.c_str());
	}

	// Root Signature 생성
	ComPtr<ID3D12Device> device = UmDevice.GetDevice();
	hr = device->CreateRootSignature(0,
									 serializedRootSignature->GetBufferPointer(),
									 serializedRootSignature->GetBufferSize(),
									 IID_PPV_ARGS(_rootSignature.GetAddressOf()));

	FAILED_CHECK_BREAK(hr);

	return S_OK;
}

HRESULT Shader::CreateRootSignature_ver0()
{
	// 24-04-07 연속된 리소스 바인딩을 위한 Root Signature 생성
	HRESULT hr = S_OK;

	std::vector<D3D12_ROOT_PARAMETER> rootParameters;
	std::vector<std::vector<D3D12_DESCRIPTOR_RANGE>> srvRanges;
	std::vector<std::vector<D3D12_DESCRIPTOR_RANGE>> uavRanges;
	std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;

	srvRanges.resize((int)Type::END);
	uavRanges.resize((int)Type::END);

	for (int i = 0; i < (int)Type::END; i++)
	{
		ComPtr<ID3D12ShaderReflection> shaderReflection;
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL;

		switch ((Type)i)
		{
		case Type::VS:
			visibility = D3D12_SHADER_VISIBILITY_VERTEX;
			break;
		case Type::PS:
			visibility = D3D12_SHADER_VISIBILITY_PIXEL;
			break;
		case Type::CS:
			visibility = D3D12_SHADER_VISIBILITY_ALL;
			break;
		}

		if (nullptr == _shaders[i]) continue;

		hr = D3DReflect(_shaders[i]->GetBufferPointer(), _shaders[i]->GetBufferSize(), IID_PPV_ARGS(shaderReflection.GetAddressOf()));
		FAILED_CHECK_BREAK(hr);

		D3D12_SHADER_DESC shaderDesc;
		shaderReflection->GetDesc(&shaderDesc);

		if ((int)Type::VS == i)
		{
			CreateInputLayout(shaderReflection, shaderDesc);
		}

		bool hasSRV = false;
		bool hasUAV = false;

		// 바인딩된 리소스 개수만큼 반복 (CBV, SRV, UAV, Sampler 등)
		for (UINT j = 0; j < shaderDesc.BoundResources; j++)
		{
			D3D12_SHADER_INPUT_BIND_DESC bindDesc;
			shaderReflection->GetResourceBindingDesc(j, &bindDesc);

			D3D12_DESCRIPTOR_RANGE descriptorRange = {};
			descriptorRange.NumDescriptors = 1;
			descriptorRange.BaseShaderRegister = bindDesc.BindPoint;
			descriptorRange.RegisterSpace = bindDesc.Space;
			descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			if (bindDesc.Type == D3D_SIT_CBUFFER) // 상수 버퍼 (CBV)
			{
				D3D12_ROOT_PARAMETER rootParam = {};
				rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				rootParam.Descriptor.ShaderRegister = bindDesc.BindPoint;
				rootParam.Descriptor.RegisterSpace = 0;
				rootParam.ShaderVisibility = visibility;
				rootParameters.push_back(rootParam);
			}
			else if (bindDesc.Type == D3D_SIT_TEXTURE ||	// 텍스처 (SRV)
				bindDesc.Type == D3D_SIT_STRUCTURED ||	// Structured Buffer (SRV)
				bindDesc.Type == D3D_SIT_BYTEADDRESS)	// ByteAddressBuffer (SRV)
			{
				hasSRV = true;
				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				srvRanges[i].push_back(descriptorRange);
			}
			else if (bindDesc.Type == D3D_SIT_UAV_RWTYPED ||		// RWTexture (UAV)
				bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED ||	// RWStructuredBuffer (UAV)
				bindDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS)	// RWByteAddressBuffer (UAV)
			{
				hasUAV = true;
				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				uavRanges[i].push_back(descriptorRange);
			}
			else if (bindDesc.Type == D3D_SIT_SAMPLER)
			{
				samplers.push_back(_staticSamplers[bindDesc.Name]);
			}
		}

		// **SRV Descriptor Table 추가 (텍스처 및 Structured Buffer)**
		if (hasSRV)
		{
			D3D12_ROOT_PARAMETER srvRootParam = {};
			srvRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			srvRootParam.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(srvRanges[i].size());
			srvRootParam.DescriptorTable.pDescriptorRanges = srvRanges[i].data();
			srvRootParam.ShaderVisibility = visibility;
			rootParameters.push_back(srvRootParam);
		}

		// **UAV Descriptor Table 추가 (RWTexture 및 RWStructuredBuffer)**
		if (hasUAV)
		{
			D3D12_ROOT_PARAMETER uavRootParam = {};
			uavRootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			uavRootParam.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(uavRanges[i].size());
			uavRootParam.DescriptorTable.pDescriptorRanges = uavRanges[i].data();
			uavRootParam.ShaderVisibility = visibility;
			rootParameters.push_back(uavRootParam);
		}
	}

	// Root Signature 생성
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.NumParameters = static_cast<UINT>(rootParameters.size());
	rootSigDesc.pParameters = rootParameters.data();
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSigDesc.NumStaticSamplers = static_cast<UINT>(samplers.size());
	rootSigDesc.pStaticSamplers = samplers.data();

	// Root Signature 직렬화
	ComPtr<ID3DBlob> serializedRootSignature;
	ComPtr<ID3DBlob> errorBlob;
	hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSignature.GetAddressOf(), errorBlob.GetAddressOf());

	std::filesystem::path errorMessage = static_cast<const char*>(errorBlob->GetBufferPointer());
	ASSERT(FAILED(hr), errorMessage.c_str());

	// Root Signature 생성
	ComPtr<ID3D12Device> device = UmDevice.GetDevice();
	hr = device->CreateRootSignature(0,
		serializedRootSignature->GetBufferPointer(),
		serializedRootSignature->GetBufferSize(),
		IID_PPV_ARGS(_rootSignature.GetAddressOf()));

	FAILED_CHECK_BREAK(hr);

	return S_OK;
}

HRESULT Shader::CompileShader(std::wstring_view filePath, std::string_view entry, std::string_view shaderModel, ComPtr<ID3DBlob>& shader)
{
	HRESULT hr = S_OK;
	ComPtr<ID3DBlob> error;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | 
				 D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;

#ifdef _DEBUG
	//디버깅 모드시 옵션 추가.
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	//셰이더 소스 컴파일.
	hr = D3DCompileFromFile(filePath.data(),
							nullptr,
							D3D_COMPILE_STANDARD_FILE_INCLUDE,	//표준 Include Files 핸들 사용.  #include Directive 사용가능 
							//nullptr,							//Include Files 핸들. 기본값은 사용않함. #include Directive 사용시 에러 발생.
							entry.data(),
							shaderModel.data(),
							flags,								//컴파일 옵션1
							0,									//컴파일 옵션2,  Effect 파일 컴파일시 적용됨. 이외에는 무시됨.
							shader.GetAddressOf(),				//[출력] 컴파일된 셰이더 코드.
							error.GetAddressOf());				//[출력] 컴파일 에러 코드.
	
	FAILED_CHECK_BREAK(hr);

	if (nullptr != error)
	{
		std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
		ASSERT(FAILED(hr), errorMessage.c_str());
	}

	return hr;
}

void Shader::CreateInputLayout(ComPtr<ID3D12ShaderReflection> shaderReflection, const D3D12_SHADER_DESC& shaderDesc)
{
	_inputElements.reserve(shaderDesc.InputParameters);
	_savedSementicNames.resize(shaderDesc.InputParameters);

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
		_savedSementicNames[i].resize(strlen(paramDesc.SemanticName) + 1);
		memcpy(_savedSementicNames[i].data(), paramDesc.SemanticName, strlen(paramDesc.SemanticName) + 1);

		D3D12_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName = _savedSementicNames[i].data();
		inputElement.SemanticIndex = paramDesc.SemanticIndex;
		inputElement.Format = format;
		inputElement.InputSlot = 0;
		inputElement.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate = 0;

		// Input Layout에 추가
		_inputElements.push_back(inputElement);
	}

	_inputLayout.NumElements = shaderDesc.InputParameters;
	_inputLayout.pInputElementDescs = _inputElements.data();
}

void Shader::CreateStaticSampler(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addressMode, UINT shaderRegister, D3D12_STATIC_SAMPLER_DESC& desc)
{
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = filter;
	samplerDesc.AddressU = addressMode;
	samplerDesc.AddressV = addressMode;
	samplerDesc.AddressW = addressMode;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = (filter == D3D12_FILTER_ANISOTROPIC) ? 16 : 0;
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc.ShaderRegister = shaderRegister;
	samplerDesc.RegisterSpace = 0;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	desc = samplerDesc;
}