#include "pch.h"
#include "Engine/GraphicsCore/ParticleEmitter.h"
#include "Engine/GraphicsCore/ParticleEffect.h"
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
     // emitter Name
     {
         std::string name = _curEmitter->GetEmitterName();
         ImGui::Text("Emitter Name");
         ImGui::SameLine();
         ImGui::InputText("##Emitter Name", &name);
         _curEmitter->SetEmitterName(name);
     }
     //shape location
     {
         if (LocationShape::SPHERE == _curEmitter->_locationType)
         {
             float locationFactor[3] = {_curEmitter->_emitLocator->GetFactor().x,
                                        _curEmitter->_emitLocator->GetFactor().y,
                                        _curEmitter->_emitLocator->GetFactor().z};

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
             _curEmitter->_emitLocator->SetFactor({locationFactor[0], locationFactor[1], locationFactor[2]});
         }
         if (LocationShape::CUBE == _curEmitter->_locationType)
         {
             float locationFactor[3] = {_curEmitter->_emitLocator->GetFactor().x,
                                        _curEmitter->_emitLocator->GetFactor().y,
                                        _curEmitter->_emitLocator->GetFactor().z};

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
             _curEmitter->_emitLocator->SetFactor({locationFactor[0], locationFactor[1], locationFactor[2]});
         }
         if (LocationShape::CYLINDER == _curEmitter->_locationType)
         {
             float locationFactor[3] = {_curEmitter->_emitLocator->GetFactor().x,
                                        _curEmitter->_emitLocator->GetFactor().y,
                                        _curEmitter->_emitLocator->GetFactor().z};

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
             _curEmitter->_emitLocator->SetFactor({locationFactor[0], locationFactor[1], locationFactor[2]});
         }
         if (LocationShape::CONE == _curEmitter->_locationType)
         {
             float locationFactor[3] = {_curEmitter->_emitLocator->GetFactor().x,
                                        _curEmitter->_emitLocator->GetFactor().y,
                                        _curEmitter->_emitLocator->GetFactor().z};

             ImGui::Text("Emitter Shape Factor");
             ImGui::Text("cone radius");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor x", &(locationFactor[0]), -10, 10);
             ImGui::Text("cone height");
             ImGui::SameLine();
             ImGui::SliderFloat("##Emitter Shape Factor y", &(locationFactor[1]), -10, 10);
             _curEmitter->_emitLocator->SetFactor({locationFactor[0], locationFactor[1], locationFactor[2]});
         }
         if (LocationShape::TORUS == _curEmitter->_locationType)
         {
             float locationFactor[3] = {_curEmitter->_emitLocator->GetFactor().x,
                                        _curEmitter->_emitLocator->GetFactor().y,
                                        _curEmitter->_emitLocator->GetFactor().z};

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
             _curEmitter->_emitLocator->SetFactor({locationFactor[0], locationFactor[1], locationFactor[2]});
         }

     }
     ImGui::Text("");

     // emitter position
     {
         float emitterPos[3] = {_curEmitter->GetEmitterPosition().x, _curEmitter->GetEmitterPosition().y,
                                _curEmitter->GetEmitterPosition().z};

         ImGui::Text("Emitter Local Position");
         ImGui::SameLine();
         ImGui::SliderFloat3("##Emitter Local Position", emitterPos, -10, 10);

         _curEmitter->SetEmitterPosition({emitterPos[0], emitterPos[1], emitterPos[2]});
     }
     // emitter rotation
     {

         Vector3 temp               = _curEmitter->GetEmitterRotationE() / XM_PI * 180.f;
         float   emitterRotation[3] = {temp.x, temp.y, temp.z};
         ImGui::Text("Emitter Local Rotation");
         ImGui::SameLine();
         ImGui::SliderFloat3("##Emitter Local Rotation", emitterRotation, -180, 180);
         _curEmitter->SetEmitterRotationE({emitterRotation[0] / 180.f * XM_PI, emitterRotation[1] / 180.f * XM_PI,
                                           emitterRotation[2] / 180.f * XM_PI});
     }
     // emitter lifetime
     {
         float lifetime = _curEmitter->GetEmitterLifetime();
         ImGui::Text("Emitter Lifetime");
         ImGui::SameLine();
         ImGui::InputFloat("##Emitter Lifetime", &lifetime);
         if (lifetime != _curEmitter->GetEmitterLifetime())
         {
             _curEmitter->Reset();
         }
         _curEmitter->SetEmitterLifetime(lifetime);
     }
     // emission rate
     {
         float rate = _curEmitter->GetEmissionRate();
         ImGui::Text("Emission Rate");
         ImGui::SameLine();
         ImGui::InputFloat("##Emission Rate", &rate);
         _curEmitter->SetEmissionRate(rate);
     }
     // spawn burst flag & count
     {
         bool spawnburstFlag = _curEmitter->GetSpawnBurstFlag();
         ImGui::Text("Spawn Burst");
         ImGui::SameLine();
         ImGui::Checkbox("##Spawn Burst", &spawnburstFlag);
         _curEmitter->SetSpawnBurstFlag(spawnburstFlag);
         if (true == spawnburstFlag)
         {
             float spawnburstCount = _curEmitter->GetSpawnBurstCount();
             ImGui::Text("Spawn Burst Count");
             ImGui::SameLine();
             ImGui::InputFloat("##Spawn Burst Count", &spawnburstCount);
             _curEmitter->SetSpawnBurstCount(spawnburstCount);
         }
     }
     ImGui::Text("");
     //particle lifetime
     {
         float lifetime = _curEmitter->GetEmitterLifetime();
         float particleLifetime = _curEmitter->GetParticleLifetime();
         ImGui::Text("Particle Lifetime");
         ImGui::SameLine();
         ImGui::InputFloat("##Particle Lifetime", &particleLifetime);
         if (particleLifetime >= lifetime)
         {
             particleLifetime = lifetime;
         }
         _curEmitter->SetParticleLifetime(particleLifetime);
     }
     ImGui::Text("");
     //velocity
     {
         static int  selected_row = -1;
         const char* items[4]     = {"Linear    ", "From Point", "In Cone   ", "Custom    "};
         static int  selected_idx = 0;

         static std::string selected_value = items[selected_idx];

         // 콤보 박스: 평소엔 선택값만, 클릭하면 확장
         
         ImGui::Text("Velocity Type");
         ImGui::SameLine();
         if (ImGui::BeginCombo("##Velocity Type", items[selected_idx]))
         {
             for (int n = 0; n < 4; n++)
             {
                 bool is_selected = (selected_idx == n);
                 if (ImGui::Selectable(items[n], is_selected))
                 {
                     selected_idx   = n;
                     selected_value = items[n]; // 선택된 값 저장
                 }
                 if (is_selected)
                     ImGui::SetItemDefaultFocus();
             }
             ImGui::EndCombo();
         }
         VelocityScaleType veltype = (VelocityScaleType)selected_idx;
         _curEmitter->SetVelocityType(veltype);
         if (VelocityScaleType::LINEAR == veltype)
         {
             Vector3 vel = _curEmitter->GetVelocityFactor();
             ImGui::Text("Velocity Scale");
             ImGui::SameLine();
             ImGui::SliderFloat3("##Velocity Scale", (float*)&vel,-1000,1000);
             _curEmitter->SetVelocityFactor(vel);
         }
         if (VelocityScaleType::POINT == veltype)
         {
             Vector3 vel = _curEmitter->GetVelocityFactor();
             float   temp = vel.x;
             ImGui::Text("Velocity Scale");
             ImGui::SameLine();
             ImGui::SliderFloat("##Velocity Scale", (float*)&temp, -1000, 1000);
             vel.x = temp;
             _curEmitter->SetVelocityFactor(vel);
         }



     }
     ImGui::Text("");
     //color & alpha
     {
         Vector3 startcolor = _curEmitter->GetStartColor();
         ImGui::Text("Start Color");
         ImGui::SameLine();
         ImGui::ColorEdit3("##Start Color", (float*)&startcolor);
         _curEmitter->SetStartColor(startcolor);

         Vector3 endcolor = _curEmitter->GetEndColor();
         ImGui::Text("End Color");
         ImGui::SameLine();
         ImGui::ColorEdit3("##End Color", (float*)&endcolor);
         _curEmitter->SetEndColor(endcolor);



         float StartAlpha = _curEmitter->GetStartOpacity();
         ImGui::Text("Start Alpha");
         ImGui::SameLine();
         ImGui::SliderFloat("##Start Alpha", &StartAlpha, 0, 1);
         _curEmitter->SetStartOpacity(StartAlpha);

         float EndAlpha = _curEmitter->GetEndOpacity();
         ImGui::Text("End Alpha");
         ImGui::SameLine(); 
         ImGui::SliderFloat("##End Alpha", &EndAlpha, 0, 1);
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
             ImGui::InputFloat2("##Start Scale", (float*)&startScale);
             startscale.x = startScale[0];
             startscale.y = startScale[1];
             _curEmitter->SetStartScale(startscale);

             Vector4 endscale    = _curEmitter->GetEndScale();
             float   endScale[2] = {endscale.x, endscale.y};
             ImGui::Text("End Scale");
             ImGui::SameLine();
             ImGui::InputFloat2("##End Scale", (float*)&endScale);
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
             ImGui::InputFloat3("##Start Scale", (float*)&startScale);
             startscale.x = startScale[0];
             startscale.y = startScale[1];
             startscale.z = startScale[2];
             _curEmitter->SetStartScale(startscale);

             Vector4 endscale    = _curEmitter->GetEndScale();
             float   endScale[3] = {endscale.x, endscale.y, endscale.z};
             ImGui::Text("End Scale");
             ImGui::SameLine();
             ImGui::InputFloat3("##End Scale", (float*)&endScale);
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
         ImGui::InputFloat("##Particle Mass", &mass);
         _curEmitter->SetParticleMass(mass);


         Vector3 offset = _curEmitter->GetParticleDistributionOffset();
         ImGui::Text("Distribution Offset");
         ImGui::SameLine();
         ImGui::InputFloat3("##Distribution Offset", (float*)&offset);
         _curEmitter->SetParticleDistributionOffset(offset);
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
