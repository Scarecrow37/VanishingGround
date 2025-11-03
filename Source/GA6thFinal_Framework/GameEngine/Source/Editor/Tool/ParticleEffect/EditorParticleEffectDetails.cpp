#include "pch.h"
#include "EditorParticleEffectDetails.h"

EditorParticleEffectDetails::EditorParticleEffectDetails()
{
    SetLabel("Details##particleeffect");
    SetDockLayout(ImGuiDir_Right);
}

EditorParticleEffectDetails::~EditorParticleEffectDetails() = default;

void EditorParticleEffectDetails::SetCurrentEffect(class ParticleEffect* curEffect)
{
    _curEffect  = curEffect;
    _curEmitter = nullptr;
}

void EditorParticleEffectDetails::SetCurrentEmitter(class ParticleEmitter* curEmitter)
{
    // 현재 매니저가 가리키는 에디터 이펙트와 동기화
    ParticleEffect* mgr = UmParticleManager->GetCurrentEditorEffect();
    if (mgr == nullptr)
    {
        _curEffect  = nullptr;
        _curEmitter = nullptr;
        return;
    }
    if (_curEffect != mgr)
    {
        _curEffect  = mgr;
        _curEmitter = nullptr;
    }

    if (curEmitter == nullptr)
    {
        _curEmitter = nullptr;
        return;
    }

    auto& list  = _curEffect->GetEmitterList(); 
    bool  found = false;
    for (auto const& up : list)
    {
        if (up.get() == curEmitter)
        {
            found = true;
            break;
        }
    }
    _curEmitter = found ? curEmitter : nullptr;
}

void EditorParticleEffectDetails::OnFrameRender()
{
    // 1) 매니저 기준으로 현재 이펙트 재동기화
    ParticleEffect* mgr = UmParticleManager->GetCurrentEditorEffect();
    if (mgr == nullptr)
    {
        _curEffect  = nullptr;
        _curEmitter = nullptr;
        ImGui::TextDisabled("No effect selected.");
        return;
    }
    if (_curEffect != mgr)
    {
        _curEffect  = mgr;
        _curEmitter = nullptr;
    }

    if (_curEmitter)
    {
        auto& list        = _curEffect->GetEmitterList();
        bool  stillExists = false;
        for (auto const& up : list)
        {
            if (up.get() == _curEmitter)
            {
                stillExists = true;
                break;
            }
        }
        if (!stillExists)
            _curEmitter = nullptr;
    }

    // 3) 렌더
    if (_curEmitter)
        ShowEmitterDetails();
    else
        ShowEffectDetails();
}

void EditorParticleEffectDetails::ShowEmitterDetails()
{
    if (nullptr == _curEmitter)
        return;

    bool isDirty = false;

    if (ImGui::BeginTable("##material", 2, ImGuiTableFlags_Borders))
    {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        const char* renderItems[3]   = {"Sprite", "Mesh  ", "Ribbon"};
        ImGui::Text("Render Type");

        ImGui::TableNextColumn();
        ImGui::Text(renderItems[static_cast<UINT>(_curEmitter->_particleType)]);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        const char* shapeItems[6] = {"Sphere      ", "Cube        ", "Cylinder    ",
                                     "Cone        ", "Torus       ", "Mesh Surface"};
        ImGui::Text("Emission Shape Type");

        ImGui::TableNextColumn();
        ImGui::Text(shapeItems[static_cast<UINT>(_curEmitter->_locationType)]);
        
        if (_curEmitter->_locationType == LocationShape::MESH_SURFACE)
        {
            ImGui::TableNextRow();
            auto pathText = _curEmitter->_emitLocator->AsMeshSurfaceLocator()->GetModelPath().string();
            ImGui::Text(pathText.c_str());
        }

        ImGui::EndTable();
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // emitter Name
    {
        std::string name = _curEmitter->GetEmitterName();
        ImGui::Text("Emitter Name");
        ImGui::SameLine();
        ImGui::InputText("##Emitter Name", &name);
        _curEmitter->SetEmitterName(name);
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // Texture
    {
        if (ParticleType::SPRITE == _curEmitter->_particleType)
        {
            if (auto spriteModule = _curEmitter->_particleRenderModule->AsSprite())
            {
                ImGui::Text("Sprite Texture");
                ImGui::SameLine();
                D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = spriteModule->GetGPUHandle();
                bool isTextureLoadButtonPressed       = ImGui::ImageButton((ImTextureID)gpuHandle.ptr, {100, 100});
                if (true == isTextureLoadButtonPressed)
                {
                    HWND                    owner = UmApplication.GetHwnd();
                    constexpr LPCWSTR       title = L"Open sprite texture";
                    std::vector<File::Path> out;
                    if (File::ShowOpenFileDialog(owner, title, L"", {{L"이미지 파일\0", L"*.jpg*\0"}}, false, out))
                    {
                        spriteModule->ChangeAlbedoTexture(out.front().wstring());
                        isDirty = true;
                        ImGui::Text("Texture Path : ");
                        ImGui::Text(out.front().string().c_str());
                    }
                }
                bool animFlag = _spriteAnimFlag;
                ImGui::Text("Sprite Animation");
                ImGui::SameLine();
                bool result = ImGui::Checkbox("##Sprite Animation", &animFlag);
                if (false == isDirty)
                    if (true == result)
                        isDirty = result;
                if (isDirty)
                    _spriteAnimFlag = animFlag;

                if (_spriteAnimFlag)
                {
                    float frameInfo[4] = {spriteModule->GetFrameInfo().x, spriteModule->GetFrameInfo().y,
                                          spriteModule->GetFrameInfo().z, spriteModule->GetFrameInfo().w};

                    bool result = ImGui::SliderFloat("u Count", &frameInfo[0], 0, 10);
                    if (false == isDirty)
                        if (true == result)
                            isDirty = result;

                    result = ImGui::SliderFloat("v Count", &frameInfo[1], 0, 10);
                    if (false == isDirty)
                        if (true == result)
                            isDirty = result;
                    
                    result = ImGui::SliderFloat("Duration", &frameInfo[3], 0, 1);
                    if (false == isDirty)
                        if (true == result)
                            isDirty = result;
                    if (isDirty)
                    {
                        frameInfo[2] = frameInfo[0] * frameInfo[1];
                        spriteModule->SetFrameInfo({frameInfo[0], frameInfo[1], frameInfo[2], frameInfo[3]});
                        spriteModule->CalculateFrameInfos();
                    }
                }

            }

        }
        if (ParticleType::RIBBON == _curEmitter->_particleType)
        {
            ImGui::Text("Ribbon Texture");
            ImGui::SameLine();
            auto                        ribbonModule = _curEmitter->_particleRenderModule->AsRibbon();
            D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle    = ribbonModule->GetGPUHandle();
            bool isTextureLoadButtonPressed          = ImGui::ImageButton((ImTextureID)gpuHandle.ptr, {100, 100});

            if (true == isTextureLoadButtonPressed)
            {
                HWND                    owner = UmApplication.GetHwnd();
                constexpr LPCWSTR       title = L"Open Ribbon texture";
                std::vector<File::Path> out;
                if (File::ShowOpenFileDialog(owner, title, L"", {{L"이미지 파일\0", L"*.jpg*\0"}}, false, out))
                {
                    ribbonModule->ChangeAlbedoTexture(out.front().wstring());
                    isDirty = true;
                }
            }
            {
                float startNormal[3] = {ribbonModule->GetStartNormal().x, ribbonModule->GetStartNormal().y,
                                        ribbonModule->GetStartNormal().z};
                ImGui::Text("ribbon start facing normal");
                bool result = ImGui::SliderFloat3("##ribbon start facing normal", startNormal, -1, 1);
                if (false == isDirty)
                    if (true == result)
                        isDirty = result;
                Vector3 temp = {startNormal[0], startNormal[1], startNormal[2]};
                temp.Normalize();

                ribbonModule->SetStartNormal({temp.x, temp.y, temp.z, 0});
            }
            {
                float endNormal[3] = {ribbonModule->GetEndNormal().x, ribbonModule->GetEndNormal().y,
                                      ribbonModule->GetEndNormal().z};
                ImGui::Text("ribbon end facing normal");
                bool result = ImGui::SliderFloat3("##ribbon end facing normal", endNormal, -1, 1);
                if (false == isDirty)
                    if (true == result)
                        isDirty = result;
                Vector3 temp = {endNormal[0], endNormal[1], endNormal[2]};
                temp.Normalize();
                ribbonModule->SetEndNormal({temp.x, temp.y, temp.z, 0});
            }
            {
                float ribbonVector[3] = {ribbonModule->GetRibbonVector().x, ribbonModule->GetRibbonVector().y,
                                         ribbonModule->GetRibbonVector().z};
                ImGui::Text("ribbon vector");
                bool result = ImGui::SliderFloat3("##ribbon vector", ribbonVector, -1, 1);
                if (false == isDirty)
                    if (true == result)
                        isDirty = result;
                Vector3 temp = {ribbonVector[0], ribbonVector[1], ribbonVector[2]};
                temp.Normalize();
                ribbonModule->SetRibbonVector({temp.x, temp.y, temp.z, 0});
            }
        }
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // shape location
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
        if (LocationShape::MESH_SURFACE == _curEmitter->_locationType)
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
        Vector3 temp = _curEmitter->_emitLocator->GetFactor();
        if (locationFactor[0] != temp.x || locationFactor[1] != temp.y || locationFactor[2] != temp.z)
            isDirty = true;
        _curEmitter->_emitLocator->SetFactor({locationFactor[0], locationFactor[1], locationFactor[2]});
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // use light
    {
        bool useLight = _curEmitter->GetUseLight();
        ImGui::Text("Use light");
        ImGui::SameLine();
        bool result = ImGui::Checkbox("##Use light", &useLight);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        if (isDirty)
            _curEmitter->SetUseLight(useLight);
        if (true == useLight)
        {
            // emitter position
            {
                float lightColor[3] = {_curEmitter->GetLightColor().x, _curEmitter->GetLightColor().y,
                                       _curEmitter->GetLightColor().z};

                ImGui::Text("Emitter Light Color");
                ImGui::SameLine();
                bool result = ImGui::ColorEdit3("##Emitter Light Color", lightColor);
                if (false == isDirty)
                    if (true == result)
                        isDirty = result;
                _curEmitter->SetLightColor({lightColor[0], lightColor[1], lightColor[2]});
            }
            // emitter light intensity
            {
                float lightIntensity = _curEmitter->GetLightIntensity();
                ImGui::Text("Emitter Light Intensity");
                ImGui::SameLine();
                bool result = ImGui::SliderFloat("##Emitter Light Intensity", &lightIntensity, 0, 10);
                if (false == isDirty)
                    if (true == result)
                        isDirty = result;
                _curEmitter->SetLightIntensity(lightIntensity);
            }
            // emitter light range
            {
                float range = _curEmitter->GetLightRange();
                ImGui::Text("Emitter Light Range");
                ImGui::SameLine();
                bool result = ImGui::SliderFloat("##Emitter Light Range", &range, 0, 100);
                if (false == isDirty)
                    if (true == result)
                        isDirty = result;
                _curEmitter->SetLightRange(range);
            }
        }
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
    // emitter position
    {
        float emitterPos[3] = {_curEmitter->GetEmitterPosition().x, _curEmitter->GetEmitterPosition().y,
                               _curEmitter->GetEmitterPosition().z};

        ImGui::Text("Emitter Local Position");
        ImGui::SameLine();
        bool result = ImGui::SliderFloat3("##Emitter Local Position", emitterPos, -10, 10);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetEmitterPosition({emitterPos[0], emitterPos[1], emitterPos[2]});
    }
    // use world space
    {
        bool useWorldSpace = _curEmitter->GetUseWorldSpace();
        ImGui::Text("Use World Space");
        ImGui::SameLine();
        bool result = ImGui::Checkbox("##Use World Space", &useWorldSpace);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetUseWorldSpace(useWorldSpace);
    }
    // emitter rotation
    {

        Vector3 temp               = _curEmitter->GetEmitterRotationE() / XM_PI * 180.f;
        float   emitterRotation[3] = {temp.x, temp.y, temp.z};
        ImGui::Text("Emitter Local Rotation");
        ImGui::SameLine();
        bool result = ImGui::SliderFloat3("##Emitter Local Rotation", emitterRotation, -180, 180);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetEmitterRotationE({emitterRotation[0] / 180.f * XM_PI, emitterRotation[1] / 180.f * XM_PI,
                                          emitterRotation[2] / 180.f * XM_PI});
    }

    // emitter lifetime
    {
        float lifetime = _curEmitter->GetEmitterLifetime();
        ImGui::Text("Emitter Lifetime");
        ImGui::SameLine();
        bool result = ImGui::InputFloat("##Emitter Lifetime", &lifetime);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetEmitterLifetime(lifetime);
    }
    // particle lifetime
    {
        float lifetime         = _curEmitter->GetEmitterLifetime();
        float particleLifetime = _curEmitter->GetParticleLifetime();
        ImGui::Text("Particle Lifetime");
        ImGui::SameLine();
        bool result = ImGui::InputFloat("##Particle Lifetime", &particleLifetime);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        if (particleLifetime >= lifetime)
        {
            particleLifetime = lifetime;
        }
        _curEmitter->SetParticleLifetime(particleLifetime);
    }

    // emission rate
    {
        float rate = _curEmitter->GetEmissionRate();
        ImGui::Text("Emission Rate");
        ImGui::SameLine();
        bool result = ImGui::InputFloat("##Emission Rate", &rate);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetEmissionRate(rate);
    }
    // spawn burst flag & count
    {
        bool spawnburstFlag = _curEmitter->GetSpawnBurstFlag();
        ImGui::Text("Spawn Burst");
        ImGui::SameLine();
        bool result = ImGui::Checkbox("##Spawn Burst", &spawnburstFlag);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetSpawnBurstFlag(spawnburstFlag);
        if (true == spawnburstFlag)
        {
            float spawnburstCount = _curEmitter->GetSpawnBurstCount();
            ImGui::Text("Spawn Burst Count");
            ImGui::SameLine();
            bool result = ImGui::InputFloat("##Spawn Burst Count", &spawnburstCount);
            if (false == isDirty)
                if (true == result)
                    isDirty = result;
            _curEmitter->SetSpawnBurstCount(spawnburstCount);
        }
    }
    // delay
    {
        float delay = _curEmitter->GetStartDelay();
        ImGui::Text("Emission Delay");
        ImGui::SameLine();
        bool result = ImGui::InputFloat("##Emission Delay", &delay);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetStartDelay(delay);
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    ImGui::Text("");
    // velocity
    {
        const char* items[4]       = {"Linear    ", "From Point", "In Cone   ", "Custom    "};
        UINT        curIdx         = (UINT)_curEmitter->_velocityType;
        std::string selectedValue = items[curIdx];

        // 콤보 박스: 평소엔 선택값만, 클릭하면 확장

        ImGui::Text("Velocity Type");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Velocity Type", items[curIdx]))
        {
            for (int n = 0; n < 4; n++)
            {
                bool isSelected = (curIdx == n);

                if (ImGui::Selectable(items[n], isSelected))
                {
                    curIdx             = n;
                    selectedValue      = items[n]; // 선택된 값 저장
                    isDirty = true;
                }
                if (isSelected)
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
            bool result = ImGui::SliderFloat3("##Velocity Scale", (float*)&vel, -1000, 1000);
            if (false == isDirty)
                if (true == result)
                    isDirty = result;
            _curEmitter->SetVelocityFactor(vel);
        }
        if (VelocityScaleType::POINT == veltype)
        {
            Vector3 vel  = _curEmitter->GetVelocityFactor();
            float   temp = vel.x;
            ImGui::Text("Velocity Scale");
            ImGui::SameLine();
            bool result = ImGui::SliderFloat("##Velocity Scale", (float*)&temp, -1000, 1000);
            if (false == isDirty)
                if (true == result)
                    isDirty = result;
            vel.x = temp;
            _curEmitter->SetVelocityFactor(vel);
        }
        if (VelocityScaleType::CONE == veltype)
        {
            Vector3 vel = _curEmitter->GetVelocityFactor();
            ImGui::Text("Velocity Scale");
            ImGui::SameLine();
            bool result = ImGui::SliderFloat3("##Velocity Scale", (float*)&vel, -1000, 1000);
            if (false == isDirty)
                if (true == result)
                    isDirty = result;
            _curEmitter->SetVelocityFactor(vel);
        }
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // color & alpha
    {
        Vector3 startcolor = _curEmitter->GetStartColor();
        ImGui::Text("Start Color");
        ImGui::SameLine();
        bool result = ImGui::ColorEdit3("##Start Color", (float*)&startcolor);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetStartColor(startcolor);

        Vector3 endcolor = _curEmitter->GetEndColor();
        ImGui::Text("End Color");
        ImGui::SameLine();
        result = ImGui::ColorEdit3("##End Color", (float*)&endcolor);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetEndColor(endcolor);

        float StartAlpha = _curEmitter->GetStartOpacity();
        ImGui::Text("Start Alpha");
        ImGui::SameLine();
        result = ImGui::SliderFloat("##Start Alpha", &StartAlpha, 0, 1);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetStartOpacity(StartAlpha);

        float EndAlpha = _curEmitter->GetEndOpacity();
        ImGui::Text("End Alpha");
        ImGui::SameLine();
        result = ImGui::SliderFloat("##End Alpha", &EndAlpha, 0, 1);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetEndOpacity(EndAlpha);
    }
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // scale
    {

        // axis
        if (ParticleType::SPRITE == _curEmitter->_particleType)
        {

            {
                Vector3 temp = _curEmitter->GetParticleRotation();
                float axis[3] = {temp.x, temp.y, temp.z};
                ImGui::Text("Particle Rotation");
                ImGui::SameLine();
                bool result = ImGui::SliderFloat3("##Particle Rotation", axis, -180, 180);
                if (false == isDirty)
                    if (true == result)
                        isDirty = result;
                _curEmitter->SetParticleRotation({axis[0], axis[1], axis[2]});
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
            }

            {
                bool scaleVelFlag = _curEmitter->GetScaleByVelocityFlag();
                ImGui::Text("Scale by Velocity");
                ImGui::SameLine();
                bool result = ImGui::Checkbox("##Scale by Velocity", &scaleVelFlag);
                if (false == isDirty)
                    if (true == result)
                        isDirty = result;
                _curEmitter->SetScaleByVelocityFlag(scaleVelFlag);

                if (false == scaleVelFlag)
                {
                    Vector3 temp = _curEmitter->GetParticleAxis();
                    temp.Normalize();
                    float axis[3] = {temp.x, temp.y, temp.z};
                    ImGui::Text("Particle Axis");
                    ImGui::SameLine();
                    bool result = ImGui::SliderFloat3("##Particle Axis", axis, -1, 1);
                    if (false == isDirty)
                        if (true == result)
                            isDirty = result;
                    _curEmitter->SetParticleAxis({axis[0], axis[1], axis[2]});
                }
            }

            Vector4 startscale    = _curEmitter->GetStartScale();
            float   startScale[2] = {startscale.x, startscale.y};
            ImGui::Text("Start Scale");
            ImGui::SameLine();
            bool result = ImGui::InputFloat2("##Start Scale", (float*)&startScale);
            if (false == isDirty)
                if (true == result)
                    isDirty = result;
            startscale.x = startScale[0];
            startscale.y = startScale[1];
            _curEmitter->SetStartScale(startscale);

            Vector4 endscale    = _curEmitter->GetEndScale();
            float   endScale[2] = {endscale.x, endscale.y};
            ImGui::Text("End Scale");
            ImGui::SameLine();
            result = ImGui::InputFloat2("##End Scale", (float*)&endScale);
            if (false == isDirty)
                if (true == result)
                    isDirty = result;
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
            if (false == isDirty)
                if (true == result)
                    isDirty = result;
            startscale.x = startScale[0];
            startscale.y = startScale[1];
            startscale.z = startScale[2];
            _curEmitter->SetStartScale(startscale);

            Vector4 endscale    = _curEmitter->GetEndScale();
            float   endScale[3] = {endscale.x, endscale.y, endscale.z};
            ImGui::Text("End Scale");
            ImGui::SameLine();
            result = ImGui::InputFloat3("##End Scale", (float*)&endScale);
            if (false == isDirty)
                if (true == result)
                    isDirty = result;
            endscale.x = endScale[0];
            endscale.y = endScale[1];
            endscale.z = endScale[2];
            _curEmitter->SetEndScale(endscale);
        }
        if (ParticleType::RIBBON == _curEmitter->_particleType)
        {

            Vector4 startscale    = _curEmitter->GetStartScale();
            float   startScale[2] = {startscale.x, startscale.y};
            ImGui::Text("Start Scale");
            ImGui::SameLine();
            bool result = ImGui::InputFloat2("##Start Scale", (float*)&startScale);
            if (false == isDirty)
                if (true == result)
                    isDirty = result;
            startscale.x = startScale[0];
            startscale.y = startScale[1];
            _curEmitter->SetStartScale(startscale);

            Vector4 endscale    = _curEmitter->GetEndScale();
            float   endScale[2] = {endscale.x, endscale.y};
            ImGui::Text("End Scale");
            ImGui::SameLine();
            result = ImGui::InputFloat2("##End Scale", (float*)&endScale);
            if (false == isDirty)
                if (true == result)
                    isDirty = result;
            endscale.x = endScale[0];
            endscale.y = endScale[1];
            _curEmitter->SetEndScale(endscale);
        }
    }

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);
    // mass & distribution
    {
        float mass = _curEmitter->GetParticleMass();
        ImGui::Text("Particle Mass");
        ImGui::SameLine();
        bool result = ImGui::InputFloat("##Particle Mass", &mass);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetParticleMass(mass);
    }
    {

        Vector3 offset = _curEmitter->GetParticleStartDistributionOffset();
        ImGui::Text("Start Distribution Offset");
        ImGui::SameLine();
        bool result = ImGui::InputFloat3("##Start Distribution Offset", (float*)&offset);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetParticleStartDistributionOffset(offset);
    }
    {
        Vector3 offset = _curEmitter->GetParticleEndDistributionOffset();
        ImGui::Text("End Distribution Offset");
        ImGui::SameLine();
        bool result = ImGui::InputFloat3("##End Distribution Offset", (float*)&offset);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetParticleEndDistributionOffset(offset);
    }
    // drag
    {
        Vector4 force = _curEmitter->GetDragForce();
        ImGui::Text("sprite loop / use world sprite scale / drag atten");
        bool result = ImGui::SliderFloat4("##Drag Force", (float*)&force, -1000, 1000);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        _curEmitter->SetDragForce(force);
    }

    // vortex
    {
        Vector4 force = _curEmitter->GetVortexForce();
        ImGui::Text("Vortex Force");
        ImGui::SameLine();
        bool result = ImGui::SliderFloat4("##Vortex Force", (float*)&force, -1000, 1000);
        if (false == isDirty)
            if (true == result)
                isDirty = result;
        if (force.Length() <= 0)
            force = {0.001f, 0.001f, 0.001f};
        _curEmitter->SetVortexForce(force);
    }

    if (true == isDirty)
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
    {
        for (auto& emitter : _curEffect->GetEmitterList())
        { 
            if (emitter && emitter->_particleRenderModule)
            {
                if (emitter->_particleRenderModule->AsSprite())
                {

                    auto& pathText = emitter->_particleRenderModule->AsSprite()->GetModelAndTexturePath();

                    int n = WideCharToMultiByte(CP_UTF8, 0, pathText.c_str(), (int)pathText.size(), nullptr, 0, nullptr,
                                                nullptr);
                    std::string pathString(n, '\0');
                    WideCharToMultiByte(CP_UTF8, 0, pathText.c_str(), (int)pathText.size(), pathString.data(), n,
                                        nullptr, nullptr);

                    std::string label = "tex : ##" + emitter->GetEmitterName() + " tex : ";
                    ImGui::InputText(label.c_str(), pathString.data(), pathString.size() + 1,
                                     ImGuiInputTextFlags_ReadOnly);
                }
                if (emitter->_particleRenderModule->AsRibbon())
                {

                    auto& pathText = emitter->_particleRenderModule->AsRibbon()->GetModelAndTexturePath();

                    int n = WideCharToMultiByte(CP_UTF8, 0, pathText.c_str(), (int)pathText.size(), nullptr, 0, nullptr,
                                                nullptr);
                    std::string pathString(n, '\0');
                    WideCharToMultiByte(CP_UTF8, 0, pathText.c_str(), (int)pathText.size(), pathString.data(), n,
                                        nullptr, nullptr);

                    std::string label = "tex : ##" + emitter->GetEmitterName() + " tex : ";
                    ImGui::InputText(label.c_str(), pathString.data(), pathString.size() + 1,
                                     ImGuiInputTextFlags_ReadOnly);
                }
                if (emitter->_locationType == LocationShape::MESH_SURFACE)
                {
                    auto        pathText = emitter->_emitLocator->AsMeshSurfaceLocator()->GetModelPath().string();
                    std::string label    = "model : ##" + emitter->GetEmitterName() + " model : ";
                    ImGui::InputText(label.c_str(), pathText.data(), pathText.size() + 1, ImGuiInputTextFlags_ReadOnly);
                }
            }
        }
    }
}
