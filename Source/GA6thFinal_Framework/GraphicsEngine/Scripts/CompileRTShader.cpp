#include <atlbase.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxcompiler.lib")

bool CompileShaderToHeader(const wchar_t* shaderFile, const wchar_t* target, const wchar_t* outputHeader,
                           const char* variableName, bool debugMode)
{
    HRESULT hr;

    // 1. DXC 컴파일러 및 라이브러리 생성
    CComPtr<IDxcCompiler>       compiler;
    CComPtr<IDxcLibrary>        library;
    CComPtr<IDxcIncludeHandler> includeHandler;

    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create DXC compiler. HRESULT: 0x" << std::hex << hr << std::endl;
        return false;
    }

    hr = DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
    if (FAILED(hr))
    {
        std::cerr << "Failed to create DXC library. HRESULT: 0x" << std::hex << hr << std::endl;
        return false;
    }

    hr = library->CreateIncludeHandler(&includeHandler);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create include handler. HRESULT: 0x" << std::hex << hr << std::endl;
        return false;
    }

    // 2. 쉐이더 파일 읽기
    std::ifstream file(shaderFile);
    if (!file.is_open())
    {
        std::wcerr << L"Failed to open shader file: " << shaderFile << std::endl;
        return false;
    }

    std::string shaderSource((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // 3. 소스를 Blob으로 변환
    CComPtr<IDxcBlobEncoding> sourceBlob;
    hr = library->CreateBlobWithEncodingFromPinned(shaderSource.c_str(), static_cast<UINT>(shaderSource.size()),
                                                   CP_UTF8, &sourceBlob);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create source blob. HRESULT: 0x" << std::hex << hr << std::endl;
        return false;
    }

    // 4. 컴파일 인자 설정
    std::vector<LPCWSTR> arguments;

    if (debugMode)
    {
        arguments.push_back(L"-Zi");
        arguments.push_back(L"-Od");
    }
    else
    {
        arguments.push_back(L"-O3");
    }

    // 5. 컴파일
    CComPtr<IDxcOperationResult> result;
    hr = compiler->Compile(sourceBlob, shaderFile, L"", target, arguments.data(), static_cast<UINT>(arguments.size()),
                           nullptr, 0, includeHandler, &result);

    if (FAILED(hr))
    {
        std::cerr << "Compile call failed. HRESULT: 0x" << std::hex << hr << std::endl;
        return false;
    }

    // 6. 컴파일 결과 확인
    HRESULT compileStatus;
    result->GetStatus(&compileStatus);

    if (FAILED(compileStatus))
    {
        CComPtr<IDxcBlobEncoding> errorBlob;
        result->GetErrorBuffer(&errorBlob);
        if (errorBlob)
        {
            std::cerr << "Compilation errors:" << std::endl;
            std::cerr << static_cast<const char*>(errorBlob->GetBufferPointer()) << std::endl;
        }
        return false;
    }

    // 7. 컴파일된 바이트코드 가져오기
    CComPtr<IDxcBlob> shaderBlob;
    result->GetResult(&shaderBlob);
    if (!shaderBlob)
    {
        std::cerr << "Failed to get shader blob" << std::endl;
        return false;
    }

    // 8. 헤더 파일로 변환
    const uint8_t* data = static_cast<const uint8_t*>(shaderBlob->GetBufferPointer());
    size_t         size = shaderBlob->GetBufferSize();

    std::ofstream outFile(outputHeader);
    if (!outFile.is_open())
    {
        std::wcerr << L"Failed to create output file: " << outputHeader << std::endl;
        return false;
    }

    outFile << "const unsigned char " << variableName << "[] = {\n    ";

    for (size_t i = 0; i < size; ++i)
    {
        outFile << static_cast<int>(data[i]);
        if (i < size - 1)
        {
            outFile << ",";
        }
        if ((i + 1) % 12 == 0 && i < size - 1)
        {
            outFile << "\n    ";
        }
    }

    outFile << "\n};";
    outFile.close();

    std::wcout << L"Success! Generated " << outputHeader << L" (" << size << L" bytes)" << std::endl;
    return true;
}

int wmain(int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        std::wcout << L"Usage: CompileRTShader.exe [debug|release]" << std::endl;
        std::wcout << L"Compiles RTShaders.hlsl to RTShaders_lib.h" << std::endl;
        return 1;
    }

    bool debugMode = (std::wstring(argv[1]) == L"debug");

    std::wcout << L"=====================================" << std::endl;
    std::wcout << L"RT Shader Compiler" << std::endl;
    std::wcout << L"Mode: " << (debugMode ? L"DEBUG" : L"RELEASE") << std::endl;
    std::wcout << L"=====================================" << std::endl;

    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path shaderPath  = currentPath / L"../../Shaders/RTShaders.hlsl";
    std::filesystem::path outputPath  = currentPath / L"../Shaders/RTShaders_lib.h";

    std::wcout << L"Input:  " << std::filesystem::absolute(shaderPath) << std::endl;
    std::wcout << L"Output: " << std::filesystem::absolute(outputPath) << std::endl;

    if (!std::filesystem::exists(shaderPath))
    {
        std::wcerr << L"Error: Shader file not found!" << std::endl;
        return 1;
    }

    bool success =
        CompileShaderToHeader(shaderPath.c_str(), L"lib_6_3", outputPath.c_str(), "g_RTShaders_lib", debugMode);

    if (!success)
    {
        std::wcerr << L"Compilation failed!" << std::endl;
        return 1;
    }

    std::wcout << L"=====================================" << std::endl;
    std::wcout << L"Compilation successful!" << std::endl;
    return 0;
}