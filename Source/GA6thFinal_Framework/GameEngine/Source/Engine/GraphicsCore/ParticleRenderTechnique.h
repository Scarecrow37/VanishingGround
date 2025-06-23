#pragma once
#include "RenderTechnique.h"
class ParticleRenderTechnique : public RenderTechnique
{

public:
    ParticleRenderTechnique()          ;
    virtual ~ParticleRenderTechnique() ;

public:
    void Initialize(ID3D12GraphicsCommandList* commandList) override;

    void Execute(ID3D12GraphicsCommandList* commandList) override;

private:
    void InitializeSpriteParticlePass();
    void InitializeParticleResolvePass();

    void CreateWBOITResources();  // 리소스 생성
    void ReleaseWBOITResources(); // 필요 시 자원 해제


    ComPtr<ID3D12Resource> _accumlateBuffer;
    ComPtr<ID3D12Resource> _revealageBuffer;

    ComPtr<ID3D12DescriptorHeap> _cpuHeap; 



    std::vector<DescriptorHandles> _oitUAVHandles;
    std::vector<DescriptorHandles> _oitUAVCpuHandles;
    std::vector<DescriptorHandles> _oitSRVHandles;




};
