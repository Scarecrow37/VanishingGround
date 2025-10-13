#include "pch.h"
#include "ShaderBuilder.h"
#include "VertexShader.h"
#include "GeometryShader.h"
#include "PixelShader.h"
#include "ComputeShader.h"

ShaderBuilder::ShaderBuilder()
    : _currentState(State::NONE)
{	
}

UINT ShaderBuilder::GetRootParameterIndex(std::string_view tag) const
{
	auto iter = _rootParameterIndex.find(tag.data());

	if (iter != _rootParameterIndex.end())
		return iter->second;

	return -1;
}

void ShaderBuilder::CreateStaticSampler(D3D12_FILTER filter, D3D12_TEXTURE_ADDRESS_MODE addressMode, D3D12_COMPARISON_FUNC func, UINT shaderRegister, D3D12_STATIC_SAMPLER_DESC& desc)
{
    D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter                    = filter;
    samplerDesc.AddressU                  = addressMode;
    samplerDesc.AddressV                  = addressMode;
    samplerDesc.AddressW                  = addressMode;
    samplerDesc.MipLODBias                = 0.0f;
    samplerDesc.MaxAnisotropy             = (filter == D3D12_FILTER_ANISOTROPIC) ? 8 : 0;
    samplerDesc.ComparisonFunc            = func;
    samplerDesc.BorderColor               = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
    samplerDesc.MinLOD                    = 0.0f;
    samplerDesc.MaxLOD                    = D3D12_FLOAT32_MAX;
    samplerDesc.ShaderRegister            = shaderRegister;
    samplerDesc.RegisterSpace             = 0;
    samplerDesc.ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    desc = samplerDesc;
}

const D3D12_INPUT_LAYOUT_DESC& ShaderBuilder::GetInputLayout() const
{
    auto vertexShader = std::static_pointer_cast<VertexShader>(_shaders[static_cast<UINT>(Type::VS)]);

    if (nullptr == vertexShader)
    {
        GRAPHICS_ASSERT(false, L"ShaderBuilder::GetInputLayout: Vertex Shader is not set");
    }

    return vertexShader->GetInputLayout();
}

void ShaderBuilder::BeginBuild()
{
	_currentState = State::BEGINE;

	_shaders.resize(static_cast<UINT>(Type::END));
	_shaderByteCodes.resize(static_cast<UINT>(Type::END));
}

void ShaderBuilder::EndBuild(BindType type)
{
    if (State::BEGINE != _currentState)
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
    {        
        switch (type)
        {
        case BindType::TABLE:
            CreateRootSignatureTable();
            break;
        case BindType::DIRECT:
            CreateRootSignatureDirect();
            break;
        }
    }

	_currentState = State::END;
}

void ShaderBuilder::SetShader(std::wstring_view filePath, ShaderBuilder::Type type)
{
	if (_shaders.empty())
		return;

	std::string_view entry;
	std::string_view shaderModel;
	UINT typeID = static_cast<UINT>(type);

	switch (type)
	{
    case ShaderBuilder::Type::VS:
        _shaders[typeID] = Global::resourceManager->LoadResource<VertexShader>(filePath.data());
		break;
    case ShaderBuilder::Type::PS:
        _shaders[typeID] = Global::resourceManager->LoadResource<PixelShader>(filePath.data());
		break;
    case ShaderBuilder::Type::GS:
        _shaders[typeID] = Global::resourceManager->LoadResource<GeometryShader>(filePath.data());
		break;
    case ShaderBuilder::Type::CS:
        _shaders[typeID] = Global::resourceManager->LoadResource<ComputeShader>(filePath.data());
		break;
	}

	_shaderByteCodes[typeID].pShaderBytecode = _shaders[typeID]->GetBufferPointer();
	_shaderByteCodes[typeID].BytecodeLength = _shaders[typeID]->GetBufferSize();
}

void ShaderBuilder::CreateRootSignatureTable()
{
    HRESULT hr = S_OK;

    std::vector<D3D12_ROOT_PARAMETER>      rootParameters;
    std::vector<D3D12_DESCRIPTOR_RANGE>    resourceRanges;
    std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;

    resourceRanges.reserve(32);

    for (int i = 0; i < (int)Type::END; i++)
    {
        if (nullptr == _shaders[i])
            continue;

        ComPtr<ID3D12ShaderReflection> shaderReflection;
        D3D12_SHADER_VISIBILITY        visibility = D3D12_SHADER_VISIBILITY_ALL;

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

        hr = D3DReflect(_shaders[i]->GetBufferPointer(), _shaders[i]->GetBufferSize(), IID_PPV_ARGS(&shaderReflection));
        FAILED_CHECK_MESSAGE(hr, L"ShaderBuilder::CreateRootSignature D3DReflect Failed");

        D3D12_SHADER_DESC shaderDesc;
        shaderReflection->GetDesc(&shaderDesc);

        if ((int)Type::VS == i)
        {
            auto vertexShader = std::static_pointer_cast<VertexShader>(_shaders[i]);
            vertexShader->CreateInputLayout(shaderReflection, shaderDesc);
        }

        // 바인딩된 리소스 개수만큼 반복 (CBV, SRV, UAV, Sampler 등)
        for (UINT j = 0; j < shaderDesc.BoundResources; j++)
        {
            D3D12_SHADER_INPUT_BIND_DESC bindDesc;
            shaderReflection->GetResourceBindingDesc(j, &bindDesc);

            D3D12_DESCRIPTOR_RANGE descriptorRange            = {};
            descriptorRange.NumDescriptors                    = 1;
            descriptorRange.BaseShaderRegister                = bindDesc.BindPoint;
            descriptorRange.RegisterSpace                     = bindDesc.Space;
            descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

            if (bindDesc.Type == D3D_SIT_CBUFFER) // 상수 버퍼 (CBV)
            {
                std::string_view     name      = bindDesc.Name;
                D3D12_ROOT_PARAMETER rootParam = {};
                if (name.find("bit32") != std::string::npos)
                {
                    size_t           first     = name.find_first_of("_");
                    size_t           end       = name.find_first_of("_", ++first);
                    std::string_view numValues = name.substr(first, end - first);

                    rootParam.ParameterType            = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
                    rootParam.Constants.Num32BitValues = std::stoi(numValues.data());
                    rootParam.Constants.ShaderRegister = bindDesc.BindPoint;
                    rootParam.Constants.RegisterSpace  = bindDesc.Space;
                    rootParam.ShaderVisibility         = visibility;
                }
                else
                {
                    rootParam.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
                    rootParam.Descriptor.ShaderRegister = bindDesc.BindPoint;
                    rootParam.Descriptor.RegisterSpace  = 0;
                    rootParam.ShaderVisibility          = visibility;
                }
                rootParameters.push_back(rootParam);
            }
            else if (bindDesc.Type == D3D_SIT_TEXTURE ||   // 텍스처 (SRV)
                     bindDesc.Type == D3D_SIT_STRUCTURED ||
                     bindDesc.Type == D3D_SIT_BYTEADDRESS) // ByteAddressBuffer (SRV)
            {
                std::string_view name = bindDesc.Name;
                if (name.find("textures") != std::string::npos)
                {
                    descriptorRange.NumDescriptors = -1;
                }                               

                descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                resourceRanges.push_back(descriptorRange);

                D3D12_ROOT_PARAMETER rootParam                = {};
                rootParam.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParam.DescriptorTable.NumDescriptorRanges = 1;
                rootParam.DescriptorTable.pDescriptorRanges   = &resourceRanges.back();
                rootParam.ShaderVisibility                    = visibility;

                rootParameters.push_back(rootParam);
            }
            else if (bindDesc.Type == D3D_SIT_UAV_RWTYPED ||      // RWTexture (UAV)
                     bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED || // RWStructuredBuffer (UAV)
                     bindDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS)  // RWByteAddressBuffer (UAV)
            {
                descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                resourceRanges.push_back(descriptorRange);

                D3D12_ROOT_PARAMETER rootParam                = {};
                rootParam.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParam.DescriptorTable.NumDescriptorRanges = 1;
                rootParam.DescriptorTable.pDescriptorRanges   = &resourceRanges.back();
                rootParam.ShaderVisibility                    = visibility;
                rootParameters.push_back(rootParam);
            }
            else if (bindDesc.Type == D3D_SIT_SAMPLER)
            {
                auto& staticSamplers = GetStaticSamplers();
                samplers.push_back(staticSamplers[bindDesc.Name]);
            }

            auto iter = _rootParameterIndex.find(bindDesc.Name);
            if (iter != _rootParameterIndex.end())
            {
                rootParameters[iter->second].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
                rootParameters.pop_back();
            }
            else
            {
                _rootParameterIndex[bindDesc.Name] = static_cast<UINT>(rootParameters.size() - 1);
            }
        }
    }

    // Root Signature 생성
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters             = static_cast<UINT>(rootParameters.size());
    rootSigDesc.pParameters               = rootParameters.data();
    rootSigDesc.Flags                     = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSigDesc.NumStaticSamplers         = static_cast<UINT>(samplers.size());
    rootSigDesc.pStaticSamplers           = samplers.data();

    // Root Signature 직렬화
    ComPtr<ID3DBlob> serializedRootSignature;
    ComPtr<ID3DBlob> errorBlob;
    hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &errorBlob);
    // FAILED_CHECK_BREAK(hr);

    if (nullptr != errorBlob)
    {
        std::filesystem::path errorMessage = static_cast<const char*>(errorBlob->GetBufferPointer());
        GRAPHICS_ASSERT(FAILED(hr), errorMessage.c_str());
    }

    // Root Signature 생성
    ID3D12Device* device = Global::device->GetDevice();
    hr                   = device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(),
                                                       serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&_rootSignature));

    FAILED_CHECK_MESSAGE(hr, L"ShaderBuilder::CreateRootSignature device->CreateRootSignature Failed");
}

void ShaderBuilder::CreateRootSignatureDirect()
{
    HRESULT hr = S_OK;

    std::vector<D3D12_ROOT_PARAMETER>      rootParameters;
    std::vector<D3D12_DESCRIPTOR_RANGE>    resourceRanges;
    std::vector<D3D12_STATIC_SAMPLER_DESC> samplers;

    resourceRanges.reserve(32);

    for (int i = 0; i < (int)Type::END; i++)
    {
        if (nullptr == _shaders[i])
            continue;

        ComPtr<ID3D12ShaderReflection> shaderReflection;
        D3D12_SHADER_VISIBILITY        visibility = D3D12_SHADER_VISIBILITY_ALL;

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

        hr = D3DReflect(_shaders[i]->GetBufferPointer(), _shaders[i]->GetBufferSize(), IID_PPV_ARGS(&shaderReflection));
        FAILED_CHECK_MESSAGE(hr, L"ShaderBuilder::CreateRootSignature D3DReflect Failed");

        D3D12_SHADER_DESC shaderDesc;
        shaderReflection->GetDesc(&shaderDesc);

        if ((int)Type::VS == i)
        {
            auto vertexShader = std::static_pointer_cast<VertexShader>(_shaders[i]);
            vertexShader->CreateInputLayout(shaderReflection, shaderDesc);
        }

        // 바인딩된 리소스 개수만큼 반복 (CBV, SRV, UAV, Sampler 등)
        for (UINT j = 0; j < shaderDesc.BoundResources; j++)
        {
            D3D12_SHADER_INPUT_BIND_DESC bindDesc;
            shaderReflection->GetResourceBindingDesc(j, &bindDesc);

            D3D12_DESCRIPTOR_RANGE descriptorRange            = {};
            descriptorRange.NumDescriptors                    = 1;
            descriptorRange.BaseShaderRegister                = bindDesc.BindPoint;
            descriptorRange.RegisterSpace                     = bindDesc.Space;
            descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

            if (bindDesc.Type == D3D_SIT_CBUFFER) // 상수 버퍼 (CBV)
            {
                std::string_view     name      = bindDesc.Name;
                D3D12_ROOT_PARAMETER rootParam = {};
                if (name.find("bit32") != std::string::npos)
                {
                    size_t           first     = name.find_first_of("_");
                    size_t           end       = name.find_first_of("_", ++first);
                    std::string_view numValues = name.substr(first, end - first);

                    rootParam.ParameterType            = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
                    rootParam.Constants.Num32BitValues = std::stoi(numValues.data());
                    rootParam.Constants.ShaderRegister = bindDesc.BindPoint;
                    rootParam.Constants.RegisterSpace  = bindDesc.Space;
                    rootParam.ShaderVisibility         = visibility;
                }
                else
                {
                    rootParam.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
                    rootParam.Descriptor.ShaderRegister = bindDesc.BindPoint;
                    rootParam.Descriptor.RegisterSpace  = bindDesc.Space;
                    rootParam.ShaderVisibility          = visibility;
                }
                rootParameters.push_back(rootParam);
            }
            else if (bindDesc.Type == D3D_SIT_STRUCTURED ||
                     bindDesc.Type == D3D_SIT_BYTEADDRESS)
            {
                D3D12_ROOT_PARAMETER rootParam      = {};
                rootParam.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
                rootParam.ShaderVisibility          = visibility;
                rootParam.Descriptor.ShaderRegister = bindDesc.BindPoint;
                rootParam.Descriptor.RegisterSpace  = bindDesc.Space;
                rootParameters.push_back(rootParam);
            }
            else if (bindDesc.Type == D3D_SIT_TEXTURE)
            {
                std::string_view name = bindDesc.Name;
                if (name.find("textures") != std::string::npos)
                {
                    descriptorRange.NumDescriptors = -1;
                }

                descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
                resourceRanges.push_back(descriptorRange);

                D3D12_ROOT_PARAMETER rootParam                = {};
                rootParam.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParam.DescriptorTable.NumDescriptorRanges = 1;
                rootParam.DescriptorTable.pDescriptorRanges   = &resourceRanges.back();
                rootParam.ShaderVisibility                    = visibility;

                rootParameters.push_back(rootParam);
            }
            else if (bindDesc.Type == D3D_SIT_UAV_RWSTRUCTURED ||
                     bindDesc.Type == D3D_SIT_UAV_RWBYTEADDRESS)
            {
                D3D12_ROOT_PARAMETER rootParam      = {};
                rootParam.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
                rootParam.ShaderVisibility          = visibility;
                rootParam.Descriptor.ShaderRegister = bindDesc.BindPoint;
                rootParam.Descriptor.RegisterSpace  = bindDesc.Space;
                rootParameters.push_back(rootParam);
            }
            else if (bindDesc.Type == D3D_SIT_UAV_RWTYPED)
            {
                descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
                resourceRanges.push_back(descriptorRange);

                D3D12_ROOT_PARAMETER rootParam                = {};
                rootParam.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
                rootParam.DescriptorTable.NumDescriptorRanges = 1;
                rootParam.DescriptorTable.pDescriptorRanges   = &resourceRanges.back();
                rootParam.ShaderVisibility                    = visibility;
                rootParameters.push_back(rootParam);
            }
            else if (bindDesc.Type == D3D_SIT_SAMPLER)
            {
                auto& staticSamplers = GetStaticSamplers();
                samplers.push_back(staticSamplers[bindDesc.Name]);
            }

            auto iter = _rootParameterIndex.find(bindDesc.Name);
            if (iter != _rootParameterIndex.end())
            {
                rootParameters[iter->second].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
                rootParameters.pop_back();
            }
            else
            {
                _rootParameterIndex[bindDesc.Name] = static_cast<UINT>(rootParameters.size() - 1);
            }
        }
    }

    // Root Signature 생성
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters             = static_cast<UINT>(rootParameters.size());
    rootSigDesc.pParameters               = rootParameters.data();
    rootSigDesc.Flags                     = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSigDesc.NumStaticSamplers         = static_cast<UINT>(samplers.size());
    rootSigDesc.pStaticSamplers           = samplers.data();

    // Root Signature 직렬화
    ComPtr<ID3DBlob> serializedRootSignature;
    ComPtr<ID3DBlob> errorBlob;
    hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSignature, &errorBlob);
    // FAILED_CHECK_BREAK(hr);

    if (nullptr != errorBlob)
    {
        std::filesystem::path errorMessage = static_cast<const char*>(errorBlob->GetBufferPointer());
        GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
    }

    // Root Signature 생성
    ID3D12Device* device = Global::device->GetDevice();
    hr                   = device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(),
                                                       serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&_rootSignature));

    FAILED_CHECK_MESSAGE(hr, L"ShaderBuilder::CreateRootSignature device->CreateRootSignature Failed");
}

std::unordered_map<std::string, D3D12_STATIC_SAMPLER_DESC>& ShaderBuilder::GetStaticSamplers()
{
    static bool initialized = false;
    static std::unordered_map<std::string, D3D12_STATIC_SAMPLER_DESC> staticSamplers;

    if (!initialized)
    {
        CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_COMPARISON_FUNC_ALWAYS, 0, staticSamplers["samPoint_wrap"]);
        CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_COMPARISON_FUNC_ALWAYS, 1, staticSamplers["samPoint_clamp"]);
        CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_COMPARISON_FUNC_ALWAYS, 2, staticSamplers["samLinear_wrap"]);
        CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_COMPARISON_FUNC_ALWAYS, 3, staticSamplers["samLinear_clamp"]);
        CreateStaticSampler(D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_COMPARISON_FUNC_ALWAYS, 4, staticSamplers["samLinear_border"]);
        CreateStaticSampler(D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_COMPARISON_FUNC_ALWAYS, 5, staticSamplers["samAnistropic_wrap"]);
        CreateStaticSampler(D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_COMPARISON_FUNC_ALWAYS, 6, staticSamplers["samAnistropic_clamp"]);
        CreateStaticSampler(D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_COMPARISON_FUNC_LESS_EQUAL, 7, staticSamplers["samComparisonLinear_border"]);
        
        initialized = true;
    }

    return staticSamplers;
}