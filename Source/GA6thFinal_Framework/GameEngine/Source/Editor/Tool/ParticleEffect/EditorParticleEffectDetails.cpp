#include "pch.h"
#include "EditorParticleEffectDetails.h"

 EditorParticleEffectDetails::EditorParticleEffectDetails() 
 {
     SetLabel("Details##particleeffect");
     SetDockLayout(ImGuiDir_Right);
 }

 EditorParticleEffectDetails::~EditorParticleEffectDetails() {}

 void EditorParticleEffectDetails::OnTickGui()
 {

 }

 void EditorParticleEffectDetails::OnStartGui()
 {

 }

 void EditorParticleEffectDetails::OnEndGui()
 {

 }

void EditorParticleEffectDetails::SetCurrentEffect(class ParticleEffect* curEffect) 
{
    _curEffect = curEffect;
    _curEmitter = nullptr;
}

 void EditorParticleEffectDetails::SetCurrentEmitter(class ParticleEmitter* curEmitter) 
{
    _curEmitter = curEmitter;
    _curEffect  = nullptr;
}

 void EditorParticleEffectDetails::OnPreFrameBegin()
 {

 }

 void EditorParticleEffectDetails::OnPostFrameBegin()
 {

     if (nullptr != _curEmitter && nullptr == _curEffect)
         ShowEmitterDetails();
     if (nullptr == _curEmitter && nullptr != _curEffect)
         ShowEffectDetails();





 }

 void EditorParticleEffectDetails::OnFrameClipped()
 {

 }

 void EditorParticleEffectDetails::OnFrameEnd()
 {

 }

 void EditorParticleEffectDetails::OnFrameFocusEnter()
 {

 }

 void EditorParticleEffectDetails::OnFrameFocusStay()
 {

 }

 void EditorParticleEffectDetails::OnFrameFocusExit()
 {

 }

 void EditorParticleEffectDetails::OnFrameRender()
 {

 }

 void EditorParticleEffectDetails::OnFramePopupOpened()
 {

 }

 void EditorParticleEffectDetails::ProcessPopupFrame()
 {

 }

 void EditorParticleEffectDetails::ProcessFocusFrame()
 {

 }

 void EditorParticleEffectDetails::ProcessRenderFrame()
 {

 }

 void EditorParticleEffectDetails::ShowEmitterDetails()
 {
     if (nullptr == _curEmitter)
         return;

     bool isSomethingChanged = false;

     ImGui::Text("");
     // emitter Name
     {
         std::string name = _curEmitter->GetEmitterName();
         ImGui::Text("Emitter Name");
         ImGui::SameLine();
         ImGui::InputText("##Emitter Name", &name);
         _curEmitter->SetEmitterName(name);
     }
     ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
     //Texture
     {
         if (ParticleType::SPRITE == _curEmitter->_particleType)
         {

             ImGui::Text("Sprite Texture");
             ImGui::SameLine();
             D3D12_GPU_DESCRIPTOR_HANDLE gpuhandle =
                 static_cast<SpriteModule*>(_curEmitter->_particleRenderModule)->GetAlbedoTexture()->GetGPUHandle();
             bool isTextureLoadButtonPressed = ImGui::ImageButton((ImTextureID)gpuhandle.ptr, {100, 100});

             if (true == isTextureLoadButtonPressed)
             {
                 HWND                    owner = UmApplication.GetHwnd();
                 LPCWSTR                 title = L"Open sprite texture";
                 std::vector<File::Path> out;
                 if (File::ShowOpenFileDialog(owner, title, L"", {{L"이미지 파일\0", L"*.jpg*\0"}}, false, out))
                 {
                     static_cast<SpriteModule*>(_curEmitter->_particleRenderModule)
                         ->ChangeAlbedoTexture(out.front().wstring());
                     isSomethingChanged = true;
                 }
             }
         }
         if (ParticleType::RIBBON == _curEmitter->_particleType)
         {

             ImGui::Text("Ribbon Texture");
             ImGui::SameLine();
             // ImGui::Button("Sprite Texture Image", {180,50});
             D3D12_GPU_DESCRIPTOR_HANDLE gpuhandle =
                 static_cast<SpriteModule*>(_curEmitter->_particleRenderModule)->GetAlbedoTexture()->GetGPUHandle();
             bool isTextureLoadButtonPressed = ImGui::ImageButton((ImTextureID)gpuhandle.ptr, {100, 100});

             if (true == isTextureLoadButtonPressed)
             {
                 HWND                    owner = UmApplication.GetHwnd();
                 LPCWSTR                 title = L"Open Ribbon texture";
                 std::vector<File::Path> out;
                 if (File::ShowOpenFileDialog(owner, title, L"", {{L"이미지 파일\0", L"*.jpg*\0"}}, false, out))
                 {
                     /*   static_cast<RibbonModule*>(_curEmitter->_particleRenderModule)
                            ->ChangeAlbedoTexture(out.front().wstring());
                        (out.front().wstring());*/
                     isSomethingChanged = true;
                 }
             }
         }
     }
     ImGui::Text("");
     //shape location
     {
         float locationFactor[3] = {_curEmitter->_emitLocator->GetFactor().x, _curEmitter->_emitLocator->GetFactor().y,
                                    _curEmitter->_emitLocator->GetFactor().z};
         if (LocationShape::SPHERE == _curEmitter->_locationType)
         {

             ImGui::Text("Emitter Shape Factor");
             ImGui::Text("X");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor x", &(locationFactor[0]), -10, 10);
             ImGui::Text("Y");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor y", &(locationFactor[1]), -10, 10);
             ImGui::Text("Z");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor z", &(locationFactor[2]), -10, 10);
         }
         if (LocationShape::CUBE == _curEmitter->_locationType)
         {

             ImGui::Text("Emitter Shape Factor");
             ImGui::Text("X");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor x", &(locationFactor[0]), -10, 10);
             ImGui::Text("Y");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor y", &(locationFactor[1]), -10, 10);
             ImGui::Text("Z");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor z", &(locationFactor[2]), -10, 10);
         }
         if (LocationShape::CYLINDER == _curEmitter->_locationType)
         {


             ImGui::Text("Emitter Shape Factor");
             ImGui::Text("X");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor x", &(locationFactor[0]), -10, 10);
             ImGui::Text("y");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor z", &(locationFactor[2]), -10, 10);
             ImGui::Text("Height");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor y", &(locationFactor[1]), -10, 10);
         }
         if (LocationShape::CONE == _curEmitter->_locationType)
         {

             ImGui::Text("Emitter Shape Factor");
             ImGui::Text("cone radius");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor x", &(locationFactor[0]), -10, 10);
             ImGui::Text("cone height");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor y", &(locationFactor[1]), -10, 10);
         }
         if (LocationShape::TORUS == _curEmitter->_locationType)
         {

             ImGui::Text("Emitter Shape Factor");
             ImGui::Text("outer radius");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor x", &(locationFactor[0]), -10, 10);
             ImGui::Text("inner radius");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor z", &(locationFactor[2]), -10, 10);
             ImGui::Text("Thickness");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor y", &(locationFactor[1]), -10, 10);
             if (locationFactor[0] <= locationFactor[2])
             {
                 locationFactor[2] = locationFactor[0] - 0.1f;
             }
         }
         Vector3 temp = _curEmitter->_emitLocator->GetFactor();
         if (locationFactor[0] != temp.x || locationFactor[1] != temp.y || locationFactor[2] != temp.z)
             isSomethingChanged = true;

         _curEmitter->_emitLocator->SetFactor({locationFactor[0], locationFactor[1], locationFactor[2]});

     }
     ImGui::Text("");

     // emitter position
     {
         float emitterPos[3] = {_curEmitter->GetEmitterPosition().x, _curEmitter->GetEmitterPosition().y,
                                _curEmitter->GetEmitterPosition().z};

         ImGui::Text("Emitter Local Position");
         ImGui::SameLine();
         bool result = ImGui::SliderFloat3("##Emitter Local Position", emitterPos, -10, 10);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         _curEmitter->SetEmitterPosition({emitterPos[0], emitterPos[1], emitterPos[2]});
     }
     // emitter rotation
     {

         Vector3 temp               = _curEmitter->GetEmitterRotationE() / XM_PI * 180.f;
         float   emitterRotation[3] = {temp.x, temp.y, temp.z};
         ImGui::Text("Emitter Local Rotation");
         ImGui::SameLine();
         bool result = ImGui::SliderFloat3("##Emitter Local Rotation", emitterRotation, -180, 180);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         _curEmitter->SetEmitterRotationE({emitterRotation[0] / 180.f * XM_PI, emitterRotation[1] / 180.f * XM_PI,
                                           emitterRotation[2] / 180.f * XM_PI});
     }
     // emitter lifetime
     {
         float lifetime = _curEmitter->GetEmitterLifetime();
         ImGui::Text("Emitter Lifetime");
         ImGui::SameLine();
         bool result = ImGui::InputFloat("##Emitter Lifetime", &lifetime);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         _curEmitter->SetEmitterLifetime(lifetime);
     }
     // emission rate
     {
         float rate = _curEmitter->GetEmissionRate();
         ImGui::Text("Emission Rate");
         ImGui::SameLine();
         bool result = ImGui::InputFloat("##Emission Rate", &rate);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         _curEmitter->SetEmissionRate(rate);
     }
     // spawn burst flag & count
     {
         bool spawnburstFlag = _curEmitter->GetSpawnBurstFlag();
         ImGui::Text("Spawn Burst");
         ImGui::SameLine();
         bool result = ImGui::Checkbox("##Spawn Burst", &spawnburstFlag);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         _curEmitter->SetSpawnBurstFlag(spawnburstFlag);
         if (true == spawnburstFlag)
         {
             float spawnburstCount = _curEmitter->GetSpawnBurstCount();
             ImGui::Text("Spawn Burst Count");
             ImGui::SameLine();
             bool result = ImGui::InputFloat("##Spawn Burst Count", &spawnburstCount);
             if (false == isSomethingChanged)
                 if (true == result)
                     isSomethingChanged = result;
             _curEmitter->SetSpawnBurstCount(spawnburstCount);
         }
     }
     // delay
     {
         float delay = _curEmitter->GetStartDelay();
         ImGui::Text("Emission Delay");
         ImGui::SameLine();
         bool result = ImGui::InputFloat("##Emission Delay", &delay);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         _curEmitter->SetStartDelay(delay);
     }



     ImGui::Text("");
     //particle lifetime
     {
         float lifetime = _curEmitter->GetEmitterLifetime();
         float particleLifetime = _curEmitter->GetParticleLifetime();
         ImGui::Text("Particle Lifetime");
         ImGui::SameLine();
         bool result = ImGui::InputFloat("##Particle Lifetime", &particleLifetime);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         if (particleLifetime >= lifetime)
         {
             particleLifetime = lifetime;
         }
         _curEmitter->SetParticleLifetime(particleLifetime);
     }
     ImGui::Text("");
     //velocity
     {
         int  selected_row = -1;
         const char* items[4]     = {"Linear    ", "From Point", "In Cone   ", "Custom    "};
         UINT        curIdx         = (UINT)_curEmitter->_velocityType;
         std::string selected_value = items[curIdx];

         // 콤보 박스: 평소엔 선택값만, 클릭하면 확장
         
         ImGui::Text("Velocity Type");
         ImGui::SameLine();
         if (ImGui::BeginCombo("##Velocity Type", items[curIdx]))
         {
             for (int n = 0; n < 4; n++)
             {
                 bool is_selected = (curIdx == n);

                 if (ImGui::Selectable(items[n], is_selected))
                 {
                     curIdx             = n;
                     selected_value = items[n]; // 선택된 값 저장
                     isSomethingChanged = true;
                 }
                 if (is_selected)
                     ImGui::SetItemDefaultFocus();
             }
             ImGui::EndCombo();
         }
         VelocityScaleType veltype = (VelocityScaleType)curIdx;
         _curEmitter->SetVelocityType(veltype);
         if (VelocityScaleType::LINEAR == veltype)
         {
             Vector3 vel = _curEmitter->GetVelocityFactor();
             ImGui::Text("Velocity Scale");
             ImGui::SameLine();
             bool result = ImGui::SliderFloat3("##Velocity Scale", (float*)&vel,-1000,1000);
             if (false == isSomethingChanged)
                 if (true == result)
                     isSomethingChanged = result;
             _curEmitter->SetVelocityFactor(vel);
         }
         if (VelocityScaleType::POINT == veltype)
         {
             Vector3 vel = _curEmitter->GetVelocityFactor();
             float   temp = vel.x;
             ImGui::Text("Velocity Scale");
             ImGui::SameLine();
             bool result = ImGui::SliderFloat("##Velocity Scale", (float*)&temp, -1000, 1000);
             if (false == isSomethingChanged)
                 if (true == result)
                     isSomethingChanged = result;
             vel.x = temp;
             _curEmitter->SetVelocityFactor(vel);
         }
         if (VelocityScaleType::CONE == veltype)
         {
             Vector3 vel = _curEmitter->GetVelocityFactor();
             ImGui::Text("Velocity Scale");
             ImGui::SameLine();
             bool result = ImGui::SliderFloat3("##Velocity Scale", (float*)&vel, -1000, 1000);
             if (false == isSomethingChanged)
                 if (true == result)
                     isSomethingChanged = result;
             _curEmitter->SetVelocityFactor(vel);
         }


     }
     ImGui::Text("");
     //color & alpha
     {
         Vector3 startcolor = _curEmitter->GetStartColor();
         ImGui::Text("Start Color");
         ImGui::SameLine();
         bool result  = ImGui::ColorEdit3("##Start Color", (float*)&startcolor);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         _curEmitter->SetStartColor(startcolor);

         Vector3 endcolor = _curEmitter->GetEndColor();
         ImGui::Text("End Color");
         ImGui::SameLine();
         result = ImGui::ColorEdit3("##End Color", (float*)&endcolor);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         _curEmitter->SetEndColor(endcolor);



         float StartAlpha = _curEmitter->GetStartOpacity();
         ImGui::Text("Start Alpha");
         ImGui::SameLine();
         result = ImGui::SliderFloat("##Start Alpha", &StartAlpha, 0, 1);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         _curEmitter->SetStartOpacity(StartAlpha);

         float EndAlpha = _curEmitter->GetEndOpacity();
         ImGui::Text("End Alpha");
         ImGui::SameLine(); 
         result = ImGui::SliderFloat("##End Alpha", &EndAlpha, 0, 1);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         _curEmitter->SetEndOpacity(EndAlpha);
     }
     ImGui::Text("");
     //scale
     {
         if (ParticleType::SPRITE == _curEmitter->_particleType)
         {

             Vector4 startscale    = _curEmitter->GetStartScale();
             float   startScale[2] = {startscale.x, startscale.y};
             ImGui::Text("Start Scale");
             ImGui::SameLine();
             bool result  = ImGui::InputFloat2("##Start Scale", (float*)&startScale);
             if (false == isSomethingChanged)
                 if (true == result)
                     isSomethingChanged = result;
             startscale.x = startScale[0];
             startscale.y = startScale[1];
             _curEmitter->SetStartScale(startscale);

             Vector4 endscale    = _curEmitter->GetEndScale();
             float   endScale[2] = {endscale.x, endscale.y};
             ImGui::Text("End Scale");
             ImGui::SameLine();
             result = ImGui::InputFloat2("##End Scale", (float*)&endScale);
             if (false == isSomethingChanged)
                 if (true == result)
                     isSomethingChanged = result;
             endscale.x = endScale[0];
             endscale.y = endScale[1];
             _curEmitter->SetEndScale(endscale);
         }
         if (ParticleType::MESH == _curEmitter->_particleType)
         {
             Vector4 startscale = _curEmitter->GetStartScale();

             float startScale[3] = {startscale.x, startscale.y, startscale.z};
             ImGui::Text("Start Scale");
             ImGui::SameLine();
             bool result = ImGui::InputFloat3("##Start Scale", (float*)&startScale);
             if (false == isSomethingChanged)
                 if (true == result)
                     isSomethingChanged = result;
             startscale.x = startScale[0];
             startscale.y = startScale[1];
             startscale.z = startScale[2];
             _curEmitter->SetStartScale(startscale);

             Vector4 endscale    = _curEmitter->GetEndScale();
             float   endScale[3] = {endscale.x, endscale.y, endscale.z};
             ImGui::Text("End Scale");
             ImGui::SameLine();
             result  = ImGui::InputFloat3("##End Scale", (float*)&endScale);
             if (false == isSomethingChanged)
                 if (true == result)
                     isSomethingChanged = result;
             endscale.x = endScale[0];
             endscale.y = endScale[1];
             endscale.z = endScale[2];
             _curEmitter->SetEndScale(endscale);
         }
     }
     ImGui::Text("");
     //mass & distribution
     {
         float mass = _curEmitter->GetParticleMass();
         ImGui::Text("Particle Mass");
         ImGui::SameLine();
         bool result = ImGui::InputFloat("##Particle Mass", &mass);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         _curEmitter->SetParticleMass(mass);


         Vector3 offset = _curEmitter->GetParticleDistributionOffset();
         ImGui::Text("Distribution Offset");
         ImGui::SameLine();
         result = ImGui::InputFloat3("##Distribution Offset", (float*)&offset);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         _curEmitter->SetParticleDistributionOffset(offset);
     }
     //drag
     {
         Vector4 force = _curEmitter->GetDragForce();
         ImGui::Text("Drag Force");
         ImGui::SameLine();
         bool result = ImGui::SliderFloat4("##Drag Force", (float*)&force, -1000, 1000);
         if (false == isSomethingChanged)
             if (true == result)
                 isSomethingChanged = result;
         _curEmitter->SetDragForce(force);
     }

     if (true == isSomethingChanged)
     {
         UmParticleManager->RefreshEditor();
     }


 }
 void EditorParticleEffectDetails::ShowEffectDetails() 
 {
     {
         std::string name = _curEffect->GetEffectName();
         ImGui::InputText("Effect Name", &name);
         _curEffect->SetEffectName(name);
     }
     {

         float lifetime = _curEffect->GetLifetime();
         ImGui::Text("Effect Lifetime");
         ImGui::SameLine();
         ImGui::InputFloat("##Effect Lifetime", &lifetime);
         _curEffect->SetLifetime(lifetime);
     }

     
 }


