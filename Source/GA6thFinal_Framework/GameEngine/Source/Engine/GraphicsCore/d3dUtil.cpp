#include "pch.h"
#include "d3dUtil.h"

Microsoft::WRL::ComPtr<ID3D12Resource> d3dUtil::CreateBufferWithData(
    ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT64 byteSize,

    Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
{
    HRESULT hr = S_OK;

    ComPtr<ID3D12Resource>  defaultBuffer;
    CD3DX12_HEAP_PROPERTIES heapPropertyDefault(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_HEAP_PROPERTIES heapPropertyUPLOAD(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC   defaultBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    CD3DX12_RESOURCE_DESC   uploadBufferDesc  = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    // 실제 기본 버퍼 자원을 생성한다
    hr = device->CreateCommittedResource(&heapPropertyDefault, D3D12_HEAP_FLAG_NONE, &defaultBufferDesc,
                                         D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&defaultBuffer));
    FAILED_CHECK_MESSAGE(hr, L"d3dUtil::CreateDefaultBuffer Failed");

    // CPU 메모리의 자료를 기본 버퍼에 복사
    // 임시 업로드 힙을 생성
    hr = device->CreateCommittedResource(&heapPropertyUPLOAD, D3D12_HEAP_FLAG_NONE, &uploadBufferDesc,
                                         D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer));
    FAILED_CHECK_MESSAGE(hr, L"d3dUtil::CreateDefaultBuffer Failed");

    // 기본 버퍼에 복사할 자료를 서술
    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData                  = initData;
    subResourceData.RowPitch               = byteSize;
    subResourceData.SlicePitch             = subResourceData.RowPitch;

    // 기본 버퍼 자원으로의 자료 복사를 요청
    // 개략적으로 말하자면, 보조함수 UpdateSubResources는 CPU 메모리를
    // 임시 업로드 힙에 복사하고, ID3D12CommandList::CopySubresourceRegion을
    // 이용해서 임시 업로드 힙의 자료를 mBuffer에 복사.
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

    cmdList->ResourceBarrier(1, &barrier);
    UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                   D3D12_RESOURCE_STATE_GENERIC_READ);
    cmdList->ResourceBarrier(1, &barrier);

    // Note: uploadBuffer has to be kept alive after the above function calls
    // because the command list has not been executed yet that performs the actual
    // copy. The caller can Release the uploadBuffer after it knows the copy has
    // been executed.

    return defaultBuffer;
}

UINT d3dUtil::AlignTo(UINT value, UINT alignment)
{
    return (((value + alignment - 1) / alignment) * alignment);
}

ComPtr<IDxcBlob> d3dUtil::CompileShaderLibrary(LPCWSTR fileName,LPCWSTR targetName)
{
    static IDxcCompiler*       pCompiler = nullptr;
    static IDxcLibrary*        pLibrary  = nullptr;
    static IDxcIncludeHandler* dxcIncludeHandler;

    HRESULT hr = S_OK;

    // Initialize the DXC compiler and compiler helper
    if (!pCompiler)
    {
        hr = DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler), reinterpret_cast<void**>(&pCompiler));
        FAILED_CHECK_MESSAGE(hr, L"d3dUtil::CompileShaderLibrary : Failed Create CLSID_DxcCompiler");
        hr = DxcCreateInstance(CLSID_DxcLibrary, __uuidof(IDxcLibrary), reinterpret_cast<void**>(&pLibrary));
        FAILED_CHECK_MESSAGE(hr, L"d3dUtil::CompileShaderLibrary : Failed Create CLSID_DxcLibrary");
        hr = pLibrary->CreateIncludeHandler(&dxcIncludeHandler);
        FAILED_CHECK_MESSAGE(hr, L"d3dUtil::CompileShaderLibrary : FAILED Create IncludeHandler");
    }
    // Open and read the file
    std::ifstream shaderFile(fileName);
    if (shaderFile.good() == false)
    {
        throw std::logic_error("Cannot find shader file");
    }
    std::stringstream strStream;
    strStream << shaderFile.rdbuf();
    std::string sShader = strStream.str();

    // Create blob from the string
    IDxcBlobEncoding* pTextBlob;
    hr = pLibrary->CreateBlobWithEncodingFromPinned(LPBYTE(sShader.c_str()), static_cast<uint32_t>(sShader.size()), 0,
                                                    &pTextBlob);
    FAILED_CHECK_MESSAGE(hr,L"d3dUtil::CompileShaderLibrary : pLibrary->CreateBlobWithEncodingFromPinned Failed");

    // Compile
    IDxcOperationResult* pResult;
    hr = pCompiler->Compile(pTextBlob, fileName, L"", targetName, nullptr, 0, nullptr, 0, dxcIncludeHandler, &pResult);
    FAILED_CHECK_MESSAGE(hr, L"d3dUtil::CompileShaderLibrary : pCompiler->Compile Failed");


    // Verify the result
    HRESULT resultCode;
    hr = pResult->GetStatus(&resultCode);
    FAILED_CHECK_MESSAGE(hr, L"d3dUtil::CompileShaderLibrary : pResult->GetStatus Failed")
    if (FAILED(resultCode))
    {
        IDxcBlobEncoding* pError;
        hr = pResult->GetErrorBuffer(&pError);
        if (FAILED(hr))
        {
            throw std::logic_error("Failed to get shader compiler error");
        }

        // Convert error blob to a string
        std::vector<char> infoLog(pError->GetBufferSize() + 1);
        memcpy(infoLog.data(), pError->GetBufferPointer(), pError->GetBufferSize());
        infoLog[pError->GetBufferSize()] = 0;

        std::string errorMsg = "Shader Compiler Error:\n";
        errorMsg.append(infoLog.data());

        MessageBoxA(nullptr, errorMsg.c_str(), "Error!", MB_OK);
        throw std::logic_error("Failed compile shader");
    }

    ComPtr<IDxcBlob> pBlob;
    hr = pResult->GetResult(pBlob.GetAddressOf());
    FAILED_CHECK_MESSAGE(hr, L"d3dUtil::CompileShaderLibrary : pBlob->GetResult Failed");
    return pBlob;
}
