#include "pch.h"
#include "GraphicsCore.h"
// 임시
#include "ParticleEffect.h"
#include "ParticleEmitter.h"
void GraphicsCore::Initialize(HWND hwnd, UINT width, UINT height, FeatureLevel feature)
{
    Device.SetUpDevice(hwnd, width, height, feature);
    ViewManager.Initialize();
    Device.Initialize(); 
    Device.ResetCommands();
    ParticleManager.Initialize(MAX_PARTICLE);
    Renderer.Initialize();

    auto commandList = Device.GetCommandList();
    commandList->Close();
    auto imguiCommandList = Device.GetImguiCommandList();
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
        ->LoadAlbedoTexture(L"../../../Resource/Assets/ParticleTexture/defaultFire.jpg");
    emitter->SetParticleLifetime(1.f);
    emitter->SetStartScale({0.5f, 0.5f, 1, 1});
    emitter->SetEndScale({0.5f, 0.5f, 1, 1});
    //emitter->SetEndScale({0.08f, 0.08f, 1, 1});
    emitter->SetStartColor({1, 0, 0});
    emitter->SetEndColor({1, 0, 0});
    emitter->SetStartOpacity(0.9f);
    emitter->SetEndOpacity(0.9f);
    emitter->SetVelocity({0, 0, 0});
    emitter->SetEmissionRate(600);
    emitter->SetLocatorFactor({1, 1, 1});
    emitter->SetParticleMass(0.f);
    emitter->SetParticleDistributionOffset(0.1f);

        auto effect1 = UmParticleManager.RegisterEffect();
    effect1->SetPosition({0, 0, 20});
    effect1->SetLifetime(15.f);
    auto emitter1 = UmParticleManager.RegisterEmitter(effect1, 100000, 1000, 20, LocationShape::SPHERE);
    emitter1->SetEmitterLifetime(15.f);
    static_cast<SpriteModule*>(emitter1->_particleRenderModule)
        ->LoadAlbedoTexture(L"../../../Resource/Assets/ParticleTexture/defaultFire.jpg");
    emitter1->SetParticleLifetime(1.f);
    emitter1->SetStartScale({5.f, 5.f, 1, 1});
    emitter1->SetEndScale({5.f, 5.f, 1, 1});
    // emitter1->SetEndScale({0.08f, 0.08f, 1, 1});
    emitter1->SetStartColor({0, 1, 0});
    emitter1->SetEndColor({0, 1, 0});
    emitter1->SetStartOpacity(0.0002f);
    emitter1->SetEndOpacity(0.0002f);
    emitter1->SetVelocity({0, 0, 0});
    emitter1->SetEmissionRate(600);
    emitter1->SetLocatorFactor({1, 1, 1});
    emitter1->SetParticleMass(0.f);
    emitter1->SetParticleDistributionOffset(1.f);

    
    //auto effect2 = UmParticleManager.RegisterEffect();
    //effect2->SetPosition({0, 0, 20});
    //effect2->SetLifetime(15.f);
    //auto emitter2 = UmParticleManager.RegisterEmitter(effect2, 100000, 1000, 20, LocationShape::SPHERE);
    //emitter2->SetEmitterLifetime(15.f);
    //static_cast<SpriteModule*>(emitter2->_particleRenderModule)
    //    ->LoadAlbedoTexture(L"../../../Resource/Assets/ParticleTexture/defaultFire.jpg");
    //emitter2->SetParticleLifetime(1.f);
    //emitter2->SetStartScale({0.5f, 0.5f, 1, 1});
    //emitter2->SetEndScale({0.5f, 0.5f, 1, 1});
    //// emitter2->SetEndScale({0.08f, 0.08f, 1, 1});
    //emitter2->SetStartColor({0, 0, 1});
    //emitter2->SetEndColor({0, 0, 1});
    //emitter2->SetStartOpacity(0.25f);
    //emitter2->SetEndOpacity(0.f);
    //emitter2->SetVelocity({0, 0, 0});
    //emitter2->SetEmissionRate(60);
    //emitter2->SetLocatorFactor({0, 0, 0});
    //emitter2->SetParticleMass(0.f);
    //emitter2->SetParticleDistributionOffset(10);




}

void GraphicsCore::UpdateAnimation(const float deltaTime)
{
    AnimationCore.Update(deltaTime);
}

void GraphicsCore::Update(const float deltaTime)
{
    ParticleManager.Update(deltaTime);
    LightCore.Update(deltaTime);
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