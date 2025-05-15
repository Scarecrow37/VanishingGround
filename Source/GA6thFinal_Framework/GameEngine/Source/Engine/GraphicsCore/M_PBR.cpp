#include "pch.h"
//#include "M_PBR.h"
//#include "Shader.h"
//
//void M_PBR::Initialize(const std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles)
//{   
//    _shader = std::make_shared<Shader>();
//
//    _shader->BeginBuild();
//    _shader->LoadShader(L"../Shaders/vs_fr.hlsl", Shader::Type::VS);
//    _shader->LoadShader(L"../Shaders/ps_pbr.hlsl", Shader::Type::PS);
//    _shader->EndBuild();
//    
//    D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};
//    psd.pRootSignature                     = _shader->GetRootSignature().Get();
//    psd.VS                                 = _shader->GetShaderByteCode(Shader::Type::VS);
//    psd.PS                                 = _shader->GetShaderByteCode(Shader::Type::PS);    
//    psd.BlendState                         = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
//    psd.SampleMask                         = UINT_MAX;
//    psd.RasterizerState                    = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
//    psd.DepthStencilState                  = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
//    psd.InputLayout                        = _shader->GetInputLayout();
//    psd.PrimitiveTopologyType              = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//    psd.NumRenderTargets                   = 1;
//    psd.RTVFormats[0]                      = DXGI_FORMAT_R32G32B32A32_FLOAT;
//    psd.DSVFormat                          = DXGI_FORMAT_D24_UNORM_S8_UINT;
//    psd.SampleDesc                         = {.Count   = UmDevice.GetMSAAState() ? (UINT)4 : (UINT)1,
//                                              .Quality = UmDevice.GetMSAAState() ? UmDevice.GetMSAAQuality() - 1 : 0};
//    
//    ComPtr<ID3D12Device> device = UmDevice.GetDevice();
//    HRESULT hr = device->CreateGraphicsPipelineState(&psd, IID_PPV_ARGS(_pipelineState.GetAddressOf()));
//    FAILED_CHECK_BREAK(hr);
//
//    D3D12_DESCRIPTOR_HEAP_DESC desc{.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
//                                    .NumDescriptors = 4,
//                                    .Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
//                                    .NodeMask       = 0};
//
//    hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(_descriptor.GetAddressOf()));
//    FAILED_CHECK_BREAK(hr);
//}
//
//void M_PBR::Instance(D3D12_GPU_VIRTUAL_ADDRESS camera)
//{
//    /*_commandAllocator.Reset();
//    _commandList->Reset(_commandAllocator.Get(), _pipelineState.Get());
//
//    _commandList->SetPipelineState(_pipelineState.Get());
//    _commandList->SetComputeRootSignature(_shader->GetRootSignature().Get());
//    _commandList->SetDescriptorHeaps(1, _descriptor.GetAddressOf());
//
//    _commandList->SetGraphicsRootConstantBufferView(_shader->GetRootSignatureIndex("cameraData"), camera);
//
//    D3D12_GPU_DESCRIPTOR_HANDLE textures = _descriptor->GetGPUDescriptorHandleForHeapStart();
//
//    _commandList->SetGraphicsRootDescriptorTable(_shader->GetRootSignatureIndex("objectData"), textures);
//    textures.ptr += UmDevice.GetCBVSRVUAVDescriptorSize();
//
//    _commandList->SetGraphicsRootDescriptorTable(_shader->GetRootSignatureIndex("material"), textures);
//    textures.ptr += UmDevice.GetCBVSRVUAVDescriptorSize();
//
//    _commandList->SetGraphicsRootDescriptorTable(_shader->GetRootSignatureIndex("textures"), textures);
//    _commandList->Close();*/
//}
//
//void M_PBR::SetupState(ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_GPU_VIRTUAL_ADDRESS camera)
//{
//    /*commandList->SetPipelineState(_pipelineState.Get());
//    commandList->SetComputeRootSignature(_shader->GetRootSignature().Get());
//    commandList->SetDescriptorHeaps(1, _descriptor.GetAddressOf());
//
//    commandList->SetGraphicsRootConstantBufferView(_shader->GetRootSignatureIndex("cameraData"), camera);
//
//    D3D12_GPU_DESCRIPTOR_HANDLE textures = _descriptor->GetGPUDescriptorHandleForHeapStart();
//
//    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootSignatureIndex("objectData"), textures);
//    textures.ptr += UmDevice.GetCBVSRVUAVDescriptorSize();
//
//    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootSignatureIndex("material"), textures);    
//    textures.ptr += UmDevice.GetCBVSRVUAVDescriptorSize();
//
//    commandList->SetGraphicsRootDescriptorTable(_shader->GetRootSignatureIndex("textures"), textures);*/
//}