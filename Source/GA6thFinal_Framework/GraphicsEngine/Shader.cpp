#include "pch.h"
#include "Shader.h"

Shader::Shader()
{
	_pDeviceContext = g_pGraphicDevice->GetDeviceContext();
}

void Shader::CompileShader(std::vector<uint8_t>& output, const std::filesystem::path& filePath, const char* entry, const char* model)
{	
	std::string path = filePath.string();
	size_t startPos = path.find_last_of('/') + 1;
	size_t endPos = path.find_last_of('.');
	path = path.substr(startPos, endPos - startPos);

	std::stringstream ss(path);
	std::string token;
	std::vector<std::string> buffer;

	while (std::getline(ss, token, '_'))
	{
		buffer.push_back(token);
	}

	std::vector<D3D_SHADER_MACRO> macros;
	for (size_t i = 1; i < buffer.size(); i++)
	{
		macros.emplace_back(buffer[i].c_str(), "");
	}

	macros.emplace_back(nullptr, nullptr);

	std::wstring shaderPath = g_shaderRootPath;
	shaderPath += filePath.filename();
	auto replaceStart = shaderPath.find_first_of('_');
	if(std::wstring::npos != replaceStart)
		shaderPath.replace(replaceStart, std::wstring::npos, L".hlsl");
	else
		shaderPath.replace(shaderPath.find_last_of('.'), std::wstring::npos, L".hlsl");
	
	ID3DBlob* pShaderBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	unsigned int compileFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	compileFlag |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	D3DCompileFromFile(shaderPath.c_str(), macros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, model, compileFlag, 0, &pShaderBlob, &pErrorBlob);
	if (pErrorBlob)
	{
		const char* errmsg = (const char*)pErrorBlob->GetBufferPointer();
		printf(errmsg);
		__debugbreak();
	}

	output.resize(pShaderBlob->GetBufferSize());
	memcpy(output.data(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize());

	std::ofstream fout;
	fout.open(filePath, std::ios::out | std::ios::binary);

	if (fout.is_open())
	{
		fout.write((const char*)output.data(), output.size());
		fout.close();
	}

	SafeRelease(pShaderBlob);
	SafeRelease(pErrorBlob);
}