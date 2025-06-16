#include "pch.h"
#include "GraphicsCore.h"
// 임시
#include "ParticleEffect.h"
#include "ParticleEmitter.h"
void GraphicsCore::Initialize(HWND hwnd, UINT width, UINT height, FEATURE_LEVEL feature)
{
    Device.SetUpDevice(hwnd, width, height, feature);
    ViewManager.Initialize();
    Device.Initialize(); 
    Device.ResetCommands();
    ParticleManager.Initialize(MAX_PARTICLE);
    Renderer.Initialize();

    auto commandList = Device.GetCommandList().Get();
    commandList->Close();
    auto imguiCommandList = Device.GetImguiCommandList().Get();
    imguiCommandList->Close();
    Device.RegisterCommand(commandList,MESH_RENDER_LIST);
    Device.ExecuteCommand(MESH_RENDER_LIST);
    Device.GPUSync();

    UmDevice.ResetCommands();
    UmDevice.ResetComputeCommands();




    //test
    auto effect = UmParticleManager.RegisterEffect();
    effect->SetPosition({0, 0, 10});
    effect->SetLifetime(15.f);
    auto emitter = UmParticleManager.RegisterEmitter(effect,100000,1000,20,LocationShape::SPHERE);
    emitter->SetEmitterLifetime(15.f);
    static_cast<SpriteModule*>(emitter->_particleRenderModule)
        ->LoadAlbedoTexture(L"../../../Resource/Assets/ParticleTexture/defaultSmoke.jpg");
    emitter->SetParticleLifetime(1.f);
    emitter->SetStartScale({0.5f, 0.5f, 1, 1});
    emitter->SetEndScale({0.5f, 0.5f, 1, 1});
    //emitter->SetEndScale({0.08f, 0.08f, 1, 1});
    emitter->SetStartColor({0, 1, 1});
    emitter->SetEndColor({0, 1, 1});
    emitter->SetStartOpacity(0.5f);
    emitter->SetEndOpacity(0.f);
    emitter->SetVelocity({0, 0, 0});
    emitter->SetEmissionRate(30);
    emitter->SetLocatorFactor({0, 0, 0});
    emitter->SetParticleMass(0.f);
    emitter->SetParticleDistributionOffset(0.1f);

    auto effect1 = UmParticleManager.RegisterEffect();
    effect1->SetPosition({0, 0, 35});
    effect1->SetLifetime(15.f);
    auto emitter1 = UmParticleManager.RegisterEmitter(effect1, 100000, 1000, 20, LocationShape::SPHERE);
    emitter1->SetEmitterLifetime(15.f);
    static_cast<SpriteModule*>(emitter1->_particleRenderModule)
        ->LoadAlbedoTexture(L"../../../Resource/Assets/ParticleTexture/defaultSmoke.jpg");
    emitter1->SetParticleLifetime(1.f);
    emitter1->SetStartScale({0.5f, 0.5f, 1, 1});
    emitter1->SetEndScale({0.5f, 0.5f, 1, 1});
    // emitter1->SetEndScale({0.08f, 0.08f, 1, 1});
    emitter1->SetStartColor({1, 0, 1});
    emitter1->SetEndColor({1, 0, 1});
    emitter1->SetStartOpacity(0.5f);
    emitter1->SetEndOpacity(0.f);
    emitter1->SetVelocity({0, 0, 0});
    emitter1->SetEmissionRate(30);
    emitter1->SetLocatorFactor({0, 0, 0});
    emitter1->SetParticleMass(0.f);
    emitter1->SetParticleDistributionOffset(0.1f);



}

void GraphicsCore::UpdateAnimation(const float deltaTime)
{
    AnimationCore.Update(deltaTime);
}

void GraphicsCore::Update()
{
    ParticleManager.Update(UmTime.DeltaTime());
    Renderer.Update();
    
}

void GraphicsCore::Render()
{
    Renderer.Render();
}

void GraphicsCore::Flip()
{
    Renderer.Flip();
}