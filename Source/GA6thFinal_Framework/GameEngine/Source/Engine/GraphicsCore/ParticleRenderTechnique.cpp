#include "pch.h"
#include "ParticleResolvePass.h"
#include "ParticleSpritePass.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "ParticleRenderTechnique.h"

 ParticleRenderTechnique::ParticleRenderTechnique() {}

 ParticleRenderTechnique::~ParticleRenderTechnique() {}

 void ParticleRenderTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{

    D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
    HRESULT hr = 
        UmDevice.GetDevice()->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options));

    if (!options.ROVsSupported)
    {

        std::filesystem::path errorMessage = static_cast<const char*>(nullptr);
        GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
    }
        

    CreateWBOITResources();
    InitializeSpriteParticlePass();
    InitializeParticleResolvePass();
}

void ParticleRenderTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    UmParticleManager.ResetRenderCommandObject();


    //CD3DX12_RESOURCE_BARRIER meshLightingBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
    //    _ownerScene->_meshLightingTarget->GetResource(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
    //    D3D12_RESOURCE_STATE_RENDER_TARGET);
    //UmParticleManager.GetRenderCommandList()->ResourceBarrier(1, &meshLightingBarrior);
    auto renderTarget = UmMultiRenderTargetManager.GetRenderTarget(_ownerScene->_finalTargetName);
    auto particleCommandList = UmParticleManager.GetRenderCommandList();
 /*   renderTarget->TransitionResource(particleCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                                     D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);*/

    __super::Execute(commandList);
    
    
    renderTarget->TransitionResource(particleCommandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        
    particleCommandList->Close();
    UmDevice.RegisterCommand(particleCommandList, PARTICLE_RENDER_LIST);
}

void ParticleRenderTechnique::InitializeSpriteParticlePass()
{
    std::unique_ptr<ParticleSpritePass> spritepass = std::make_unique<ParticleSpritePass>();
    spritepass->SetOwnerScene(_ownerScene);
    D3D12_VIEWPORT viewport{.TopLeftX = 0,
                            .TopLeftY = 0,
                            .Width    = (FLOAT)UmDevice.GetMode().Width,
                            .Height   = (FLOAT)UmDevice.GetMode().Height,
                            .MinDepth = 0.f,
                            .MaxDepth = 1.f};
    D3D12_RECT     scissor{
            .left = 0, .top = 0, .right = (LONG)UmDevice.GetMode().Width, .bottom = (LONG)UmDevice.GetMode().Height};
    spritepass->Initialize(viewport, scissor);
    spritepass->SetAccumulationBuffers(_accumlateBuffer, _revealageBuffer, _oitUAVHandles,_oitUAVCpuHandles);
    AddRenderPass(std::move(spritepass));
}

void ParticleRenderTechnique::InitializeParticleResolvePass()
{
    std::unique_ptr<ParticleResolvePass> resolvepass = std::make_unique<ParticleResolvePass>();
    resolvepass->SetOwnerScene(_ownerScene);
    D3D12_VIEWPORT viewport{.TopLeftX = 0,
                            .TopLeftY = 0,
                            .Width    = (FLOAT)UmDevice.GetMode().Width,
                            .Height   = (FLOAT)UmDevice.GetMode().Height,
                            .MinDepth = 0.f,
                            .MaxDepth = 1.f};
    D3D12_RECT     scissor{
            .left = 0, .top = 0, .right = (LONG)UmDevice.GetMode().Width, .bottom = (LONG)UmDevice.GetMode().Height};
    resolvepass->Initialize(viewport, scissor);
    resolvepass->SetAccumulationBuffers(_accumlateBuffer, _revealageBuffer, _oitSRVHandles);
    AddRenderPass(std::move(resolvepass));
}

void ParticleRenderTechnique::CreateWBOITResources()
{
    UINT width  = UmDevice.GetMode().Width;
    UINT height = UmDevice.GetMode().Height;

    auto createTex = [&](DXGI_FORMAT format) -> ComPtr<ID3D12Resource> {
        D3D12_RESOURCE_DESC desc = {};
        desc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Width               = width ;
        desc.Height              = height;
        desc.DepthOrArraySize    = 1;
        desc.MipLevels           = 1;
        desc.Format              = format;
        desc.SampleDesc.Count    = 1;
        desc.Layout              = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        desc.Flags               = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);


        ComPtr<ID3D12Resource> tex;
        HRESULT                hr = UmDevice.GetDevice()->CreateCommittedResource(
            &heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&tex));
        FAILED_CHECK_MESSAGE(hr, L"WBOIT UAV 생성 실패");
        return tex;
    };

    _accumlateBuffer = createTex(DXGI_FORMAT_R16G16B16A16_FLOAT);
    _revealageBuffer = createTex(DXGI_FORMAT_R16_FLOAT);

    _oitUAVHandles.resize(2);
    _oitSRVHandles.resize(2);
    _oitUAVCpuHandles.resize(2);




    D3D12_UNORDERED_ACCESS_VIEW_DESC accumlateUAVDesc = {};
    accumlateUAVDesc.Format                           = DXGI_FORMAT_R16G16B16A16_FLOAT; // 텍스처 포맷 지정[3]
    accumlateUAVDesc.ViewDimension                    = D3D12_UAV_DIMENSION_TEXTURE2D;  // 2D 텍스처 뷰[3]
    accumlateUAVDesc.Texture2D.MipSlice               = 0;                              // 첫 번째 밉 레벨[3]
    accumlateUAVDesc.Texture2D.PlaneSlice             = 0;                              // 첫 번째 플레인[3]
    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _oitUAVHandles[0]);
    UmDevice.GetDevice()->CreateUnorderedAccessView(_accumlateBuffer.Get(), nullptr, &accumlateUAVDesc,
                                                    _oitUAVHandles[0].CPU);

    D3D12_UNORDERED_ACCESS_VIEW_DESC revealageUAVDesc        = {};
    revealageUAVDesc.Format                           = DXGI_FORMAT_R16_FLOAT;         // 텍스처 포맷 지정[3]
    revealageUAVDesc.ViewDimension                    = D3D12_UAV_DIMENSION_TEXTURE2D; // 2D 텍스처 뷰[3]
    revealageUAVDesc.Texture2D.MipSlice               = 0;                             // 첫 번째 밉 레벨[3]
    revealageUAVDesc.Texture2D.PlaneSlice             = 0;                             // 첫 번째 플레인[3]
    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _oitUAVHandles[1]);
    UmDevice.GetDevice()->CreateUnorderedAccessView(_revealageBuffer.Get(), nullptr, &revealageUAVDesc,
                                                    _oitUAVHandles[1].CPU);


        // cpu heap for clear accum/reveal buff
    D3D12_DESCRIPTOR_HEAP_DESC cpuHeapDesc = {};
    cpuHeapDesc.Type                       = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cpuHeapDesc.NumDescriptors             = 2; // 필요한 UAV 슬롯 수
    cpuHeapDesc.Flags                      = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    cpuHeapDesc.NodeMask                   = 0;
    UmDevice.GetDevice()->CreateDescriptorHeap(&cpuHeapDesc, IID_PPV_ARGS(&_cpuHeap));

    auto cpuheapstart = _cpuHeap->GetCPUDescriptorHandleForHeapStart();


    _oitUAVCpuHandles[0].CPU = cpuheapstart;
    cpuheapstart.ptr += UmDevice.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    _oitUAVCpuHandles[1].CPU = cpuheapstart;

    UmDevice.GetDevice()->CreateUnorderedAccessView(_accumlateBuffer.Get(), // 리소스
                                                    nullptr, &accumlateUAVDesc, _oitUAVCpuHandles[0].CPU);

    UmDevice.GetDevice()->CreateUnorderedAccessView(_revealageBuffer.Get(), // 리소스
                                                    nullptr, &revealageUAVDesc, _oitUAVCpuHandles[1].CPU);






    D3D12_SHADER_RESOURCE_VIEW_DESC accumlateSRVDesc = {};
    accumlateSRVDesc.Format                  = DXGI_FORMAT_R16G16B16A16_FLOAT; // 텍스처 요소당 32비트 unsigned 정수[8]
    accumlateSRVDesc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;  // 2D 텍스처 뷰 지정[8]
    accumlateSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    accumlateSRVDesc.Texture2D.MostDetailedMip = 0; // 가장 상세한 mip-level 지정[8]
    accumlateSRVDesc.Texture2D.MipLevels = accumlateSRVDesc.Texture2D.MipLevels = -1;
    accumlateSRVDesc.Texture2D.PlaneSlice                                       = 0;    // 첫 번째 플레인 선택[8]
    accumlateSRVDesc.Texture2D.ResourceMinLODClamp                              = 0.0f; // 최소 LOD 클램프[8]
    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _oitSRVHandles[0]);
    UmDevice.GetDevice()->CreateShaderResourceView(_accumlateBuffer.Get(), &accumlateSRVDesc, _oitSRVHandles[0].CPU);

    D3D12_SHADER_RESOURCE_VIEW_DESC revealageSRVDesc = {};
    revealageSRVDesc.Format                          = DXGI_FORMAT_R16_FLOAT; // 텍스처 요소당 32비트 unsigned 정수[8]
    revealageSRVDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D; // 2D 텍스처 뷰 지정[8]
    revealageSRVDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    // 기본 컴포넌트 매핑[8]
    revealageSRVDesc.Texture2D.MostDetailedMip = 0; // 가장 상세한 mip-level 지정[8]
    revealageSRVDesc.Texture2D.MipLevels = revealageSRVDesc.Texture2D.MipLevels = -1;
    revealageSRVDesc.Texture2D.PlaneSlice                                       = 0;    // 첫 번째 플레인 선택[8]
    revealageSRVDesc.Texture2D.ResourceMinLODClamp                              = 0.0f; // 최소 LOD 클램프[8]
    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _oitSRVHandles[1]);
    UmDevice.GetDevice()->CreateShaderResourceView(_revealageBuffer.Get(), &revealageSRVDesc, _oitSRVHandles[1].CPU);

}



void ParticleRenderTechnique::ReleaseWBOITResources() {}
