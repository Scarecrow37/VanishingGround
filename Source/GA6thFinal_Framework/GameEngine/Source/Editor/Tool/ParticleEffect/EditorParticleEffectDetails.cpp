#include "EditorParticleEffectDetails.h"
#include "pch.h"

// ──────────────────────────────────────────────────────────────────────────────
// Life-cycle
// ──────────────────────────────────────────────────────────────────────────────
EditorParticleEffectDetails::EditorParticleEffectDetails()
{
    SetLabel("Details##particleeffect");
    SetDockLayout(ImGuiDir_Right);
}

EditorParticleEffectDetails::~EditorParticleEffectDetails() {}

void EditorParticleEffectDetails::OnTickGui() {}
void EditorParticleEffectDetails::OnStartGui() {}
void EditorParticleEffectDetails::OnEndGui() {}
void EditorParticleEffectDetails::OnPreFrameBegin() {}
void EditorParticleEffectDetails::OnFrameClipped() {}
void EditorParticleEffectDetails::OnFrameEnd() {}
void EditorParticleEffectDetails::OnFrameFocusEnter() {}
void EditorParticleEffectDetails::OnFrameFocusStay() {}
void EditorParticleEffectDetails::OnFrameFocusExit() {}
void EditorParticleEffectDetails::OnFrameRender() {}
void EditorParticleEffectDetails::OnFramePopupOpened() {}
void EditorParticleEffectDetails::ProcessPopupFrame() {}
void EditorParticleEffectDetails::ProcessFocusFrame() {}
void EditorParticleEffectDetails::ProcessRenderFrame() {}

// ──────────────────────────────────────────────────────────────────────────────
void EditorParticleEffectDetails::SetCurrentEffect(class ParticleEffect* curEffect)
{
    _curEffect  = curEffect;
    _curEmitter = nullptr;
}

void EditorParticleEffectDetails::SetCurrentEmitter(class ParticleEmitter* curEmitter)
{
    _curEmitter = curEmitter;
    _curEffect  = nullptr;
}

// ──────────────────────────────────────────────────────────────────────────────
// Frame dispatch
// ──────────────────────────────────────────────────────────────────────────────
void EditorParticleEffectDetails::OnPostFrameBegin()
{
    // [Guard] Manager / CurrentEditorEffect nullptr 가능
    if (nullptr == UmParticleManager || nullptr == UmParticleManager->GetCurrentEditorEffect())
    {
        _curEffect = nullptr;
        return;
    }

    if (nullptr != _curEmitter && nullptr == _curEffect)
        ShowEmitterDetails();

    if (nullptr == _curEmitter && nullptr != _curEffect)
        ShowEffectDetails();
}

// ──────────────────────────────────────────────────────────────────────────────
// Emitter pane
// ──────────────────────────────────────────────────────────────────────────────
void EditorParticleEffectDetails::ShowEmitterDetails()
{
    // [Guard] 선택된 emitter 없음
    if (nullptr == _curEmitter)
        return;

    bool isSomethingChanged = false;

    ImGui::Text("");

    // ── [Emitter Name] ──────────────────────────────────────────────────────
    {
        std::string name = _curEmitter->GetEmitterName();
        ImGui::Text("Emitter Name");
        ImGui::SameLine();
        ImGui::InputText("##Emitter Name", &name);
        _curEmitter->SetEmitterName(name);
    }

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // ── [Texture / Module-specific] ─────────────────────────────────────────
    {
        if (ParticleType::SPRITE == _curEmitter->_particleType)
        {
            // [Guard] 모듈/텍스처 nullptr 가능
            auto spriteModule = (nullptr != _curEmitter->_particleRenderModule)
                                    ? _curEmitter->_particleRenderModule->AsSprite()
                                    : nullptr;
            if (nullptr != spriteModule && nullptr != spriteModule->GetAlbedoTexture())
            {
                ImGui::Text("Sprite Texture");
                ImGui::SameLine();
                D3D12_GPU_DESCRIPTOR_HANDLE gpuhandle = spriteModule->GetAlbedoTexture()->GetGPUHandle();
                bool                        isTextureLoadButtonPressed = ImGui::ImageButton(
                    (ImTextureID)gpuhandle.ptr, ImVec2(_uiTexturePreviewWidth, _uiTexturePreviewHeight));

                if (true == isTextureLoadButtonPressed)
                {
                    HWND                    owner = UmApplication.GetHwnd();
                    LPCWSTR                 title = L"Open sprite texture";
                    std::vector<File::Path> out;
                    if (File::ShowOpenFileDialog(owner, title, L"", {{L"이미지 파일\0", L"*.jpg*\0"}}, false, out))
                    {
                        // [Guard] 선택 파일 비어있을 수 있음
                        if (false == out.empty())
                        {
                            spriteModule->ChangeAlbedoTexture(out.front().wstring());
                            isSomethingChanged = true;
                        }
                    }
                }
            }
        }

        if (ParticleType::RIBBON == _curEmitter->_particleType)
        {
            ImGui::Text("Ribbon Texture");
            ImGui::SameLine();

            // [Guard] 모듈/텍스처 nullptr 가능
            auto ribbonModule = (nullptr != _curEmitter->_particleRenderModule)
                                    ? _curEmitter->_particleRenderModule->AsRibbon()
                                    : nullptr;

            if (nullptr != ribbonModule && nullptr != ribbonModule->GetAlbedoTexture())
            {
                D3D12_GPU_DESCRIPTOR_HANDLE gpuhandle = ribbonModule->GetAlbedoTexture()->GetGPUHandle();
                bool                        isTextureLoadButtonPressed = ImGui::ImageButton(
                    (ImTextureID)gpuhandle.ptr, ImVec2(_uiTexturePreviewWidth, _uiTexturePreviewHeight));

                if (true == isTextureLoadButtonPressed)
                {
                    HWND                    owner = UmApplication.GetHwnd();
                    LPCWSTR                 title = L"Open Ribbon texture";
                    std::vector<File::Path> out;
                    if (File::ShowOpenFileDialog(owner, title, L"", {{L"이미지 파일\0", L"*.jpg*\0"}}, false, out))
                    {
                        if (false == out.empty())
                        {
                            ribbonModule->ChangeAlbedoTexture(out.front().wstring());
                            isSomethingChanged = true;
                        }
                    }
                }
            }

            if (nullptr != ribbonModule) // [Guard]
            {
                // Start Normal
                {
                    float startnormal[3] = {ribbonModule->GetStartNormal().x, ribbonModule->GetStartNormal().y,
                                            ribbonModule->GetStartNormal().z};
                    ImGui::Text("ribbon start facing normal");
                    bool result = ImGui::SliderFloat3("##ribbon start facing normal", startnormal, -1.0f, 1.0f);
                    if (false == isSomethingChanged)
                        if (true == result)
                            isSomethingChanged = result;

                    Vector3 temp = {startnormal[0], startnormal[1], startnormal[2]};
                    temp.Normalize();
                    ribbonModule->SetStartNormal({temp.x, temp.y, temp.z, 0});
                }

                // End Normal
                {
                    float endnormal[3] = {ribbonModule->GetEndNormal().x, ribbonModule->GetEndNormal().y,
                                          ribbonModule->GetEndNormal().z};
                    ImGui::Text("ribbon end facing normal");
                    bool result = ImGui::SliderFloat3("##ribbon end facing normal", endnormal, -1.0f, 1.0f);
                    if (false == isSomethingChanged)
                        if (true == result)
                            isSomethingChanged = result;

                    Vector3 temp = {endnormal[0], endnormal[1], endnormal[2]};
                    temp.Normalize();
                    ribbonModule->SetEndNormal({temp.x, temp.y, temp.z, 0});
                }

                // Ribbon Vector
                {
                    float ribbonvector[3] = {ribbonModule->GetRibbonVector().x, ribbonModule->GetRibbonVector().y,
                                             ribbonModule->GetRibbonVector().z};
                    ImGui::Text("ribbon vector");
                    bool result = ImGui::SliderFloat3("##ribbon vector", ribbonvector, -1.0f, 1.0f);
                    if (false == isSomethingChanged)
                        if (true == result)
                            isSomethingChanged = result;

                    Vector3 temp = {ribbonvector[0], ribbonvector[1], ribbonvector[2]};
                    temp.Normalize();
                    ribbonModule->SetRibbonVector({temp.x, temp.y, temp.z, 0});
                }
            }
        }
    }

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // ── [Location Shape Factor] ─────────────────────────────────────────────
    {
        // [Guard] 로케이터 nullptr 가능
        Vector3 current =
            (nullptr != _curEmitter->_emitLocator) ? _curEmitter->_emitLocator->GetFactor() : Vector3(0, 0, 0);

        float locationFactor[3] = {current.x, current.y, current.z};

        if (LocationShape::SPHERE == _curEmitter->_locationType)
        {
            ImGui::Text("Emitter Shape Factor");
            ImGui::Text("X");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor x", &(locationFactor[0]), _uiShapeMin, _uiShapeMax);
            ImGui::Text("Y");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor y", &(locationFactor[1]), _uiShapeMin, _uiShapeMax);
            ImGui::Text("Z");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor z", &(locationFactor[2]), _uiShapeMin, _uiShapeMax);
        }
        if (LocationShape::CUBE == _curEmitter->_locationType)
        {
            ImGui::Text("Emitter Shape Factor");
            ImGui::Text("X");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor x", &(locationFactor[0]), _uiShapeMin, _uiShapeMax);
            ImGui::Text("Y");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor y", &(locationFactor[1]), _uiShapeMin, _uiShapeMax);
            ImGui::Text("Z");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor z", &(locationFactor[2]), _uiShapeMin, _uiShapeMax);
        }
        if (LocationShape::CYLINDER == _curEmitter->_locationType)
        {
            ImGui::Text("Emitter Shape Factor");
            ImGui::Text("X");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor x", &(locationFactor[0]), _uiShapeMin, _uiShapeMax);
            ImGui::Text("y");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor z", &(locationFactor[2]), _uiShapeMin, _uiShapeMax);
            ImGui::Text("Height");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor y", &(locationFactor[1]), _uiShapeMin, _uiShapeMax);
        }
        if (LocationShape::CONE == _curEmitter->_locationType)
        {
            ImGui::Text("Emitter Shape Factor");
            ImGui::Text("cone radius");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor x", &(locationFactor[0]), _uiShapeMin, _uiShapeMax);
            ImGui::Text("cone height");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor y", &(locationFactor[1]), _uiShapeMin, _uiShapeMax);
        }
        if (LocationShape::TORUS == _curEmitter->_locationType)
        {
            ImGui::Text("Emitter Shape Factor");
            ImGui::Text("outer radius");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor x", &(locationFactor[0]), _uiShapeMin, _uiShapeMax);
            ImGui::Text("inner radius");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor z", &(locationFactor[2]), _uiShapeMin, _uiShapeMax);
            ImGui::Text("Thickness");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor y", &(locationFactor[1]), _uiShapeMin, _uiShapeMax);

            if (locationFactor[0] <= locationFactor[2])
            {
                locationFactor[2] = locationFactor[0] - _uiTorusInnerMargin;
            }
        }
        if (LocationShape::MESH_SURFACE == _curEmitter->_locationType)
        {
            ImGui::Text("Emitter Shape Factor");
            ImGui::Text("X");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor x", &(locationFactor[0]), _uiShapeMin, _uiShapeMax);
            ImGui::Text("Y");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor y", &(locationFactor[1]), _uiShapeMin, _uiShapeMax);
            ImGui::Text("Z");
            ImGui::SameLine();
            ImGui::SliderFloat("##Emitter Shape Factor z", &(locationFactor[2]), _uiShapeMin, _uiShapeMax);
        }

        if (locationFactor[0] != current.x || locationFactor[1] != current.y || locationFactor[2] != current.z)
            isSomethingChanged = true;

        if (nullptr != _curEmitter->_emitLocator) // [Guard]
            _curEmitter->_emitLocator->SetFactor({locationFactor[0], locationFactor[1], locationFactor[2]});
    }

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // ── [Lighting Settings] ─────────────────────────────────────────────────
    {
        bool uselight = _curEmitter->GetUseLight();
        ImGui::Text("Use light");
        ImGui::SameLine();
        bool result = ImGui::Checkbox("##Use light", &uselight);
        if (false == isSomethingChanged)
            if (true == result)
                isSomethingChanged = result;

        if (true == uselight)
        {
            // Light Color
            {
                float lightcolor[3] = {_curEmitter->GetLightColor().x, _curEmitter->GetLightColor().y,
                                       _curEmitter->GetLightColor().z};

                ImGui::Text("Emitter Light Color");
                ImGui::SameLine();
                bool result = ImGui::ColorEdit3("##Emitter Light Color", lightcolor);
                if (false == isSomethingChanged)
                    if (true == result)
                        isSomethingChanged = result;

                _curEmitter->SetLightColor({lightcolor[0], lightcolor[1], lightcolor[2]});
            }
            // Light Intensity
            {
                float lightintensity = _curEmitter->GetLightIntensity();
                ImGui::Text("Emitter Light Intensity");
                ImGui::SameLine();
                bool result =
                    ImGui::SliderFloat("##Emitter Light Intensity", &lightintensity, _uiIntensityMin, _uiIntensityMax);
                if (false == isSomethingChanged)
                    if (true == result)
                        isSomethingChanged = result;

                _curEmitter->SetLightIntensity(lightintensity);
            }
            // Light Range
            {
                float range = _curEmitter->GetLightRange();
                ImGui::Text("Emitter Light Range");
                ImGui::SameLine();
                bool result = ImGui::SliderFloat("##Emitter Light Range", &range, _uiRangeMin, _uiRangeMax);
                if (false == isSomethingChanged)
                    if (true == result)
                        isSomethingChanged = result;

                _curEmitter->SetLightRange(range);
            }
        }
    }

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // ── [Emitter Transform] ─────────────────────────────────────────────────
    {
        // Position
        {
            float emitterPos[3] = {_curEmitter->GetEmitterPosition().x, _curEmitter->GetEmitterPosition().y,
                                   _curEmitter->GetEmitterPosition().z};

            ImGui::Text("Emitter Local Position");
            ImGui::SameLine();
            bool result = ImGui::SliderFloat3("##Emitter Local Position", emitterPos, _uiShapeMin, _uiShapeMax);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;

            _curEmitter->SetEmitterPosition({emitterPos[0], emitterPos[1], emitterPos[2]});
        }
        // Use World Space
        {
            bool useWorldSpace = _curEmitter->GetUseWorldSpace();
            ImGui::Text("Use World Space");
            ImGui::SameLine();
            bool result = ImGui::Checkbox("##Use World Space", &useWorldSpace);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;

            _curEmitter->SetUseWorldSpace(useWorldSpace);
        }
        // Rotation (degrees UI)
        {
            Vector3 temp               = _curEmitter->GetEmitterRotationE() / XM_PI * 180.f;
            float   emitterRotation[3] = {temp.x, temp.y, temp.z};
            ImGui::Text("Emitter Local Rotation");
            ImGui::SameLine();
            bool result = ImGui::SliderFloat3("##Emitter Local Rotation", emitterRotation, _uiRotDegMin, _uiRotDegMax);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;

            _curEmitter->SetEmitterRotationE({emitterRotation[0] / 180.f * XM_PI, emitterRotation[1] / 180.f * XM_PI,
                                              emitterRotation[2] / 180.f * XM_PI});
        }
    }

    // ── [Lifetime / Emission Settings] ──────────────────────────────────────
    {
        // Emitter lifetime
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
        // Particle lifetime
        {
            float lifetime         = _curEmitter->GetEmitterLifetime();
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
        // Emission rate
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
        // Spawn burst flag/count
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
        // Delay
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
    }

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // ── [Velocity Settings] ─────────────────────────────────────────────────
    {
        int         selected_row   = -1;
        const char* items[4]       = {"Linear    ", "From Point", "In Cone   ", "Custom    "};
        UINT        curIdx         = (UINT)_curEmitter->_velocityType;
        std::string selected_value = items[curIdx];

        ImGui::Text("Velocity Type");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Velocity Type", items[curIdx]))
        {
            for (int n = 0; n < 4; n++)
            {
                bool is_selected = (curIdx == (UINT)n);
                if (ImGui::Selectable(items[n], is_selected))
                {
                    curIdx             = (UINT)n;
                    selected_value     = items[n];
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
            bool result = ImGui::SliderFloat3("##Velocity Scale", (float*)&vel, _uiVelScaleMin, _uiVelScaleMax);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;
            _curEmitter->SetVelocityFactor(vel);
        }
        if (VelocityScaleType::POINT == veltype)
        {
            Vector3 vel  = _curEmitter->GetVelocityFactor();
            float   temp = vel.x;
            ImGui::Text("Velocity Scale");
            ImGui::SameLine();
            bool result = ImGui::SliderFloat("##Velocity Scale", (float*)&temp, _uiVelScaleMin, _uiVelScaleMax);
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
            bool result = ImGui::SliderFloat3("##Velocity Scale", (float*)&vel, _uiVelScaleMin, _uiVelScaleMax);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;
            _curEmitter->SetVelocityFactor(vel);
        }
    }

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // ── [Color & Alpha] ─────────────────────────────────────────────────────
    {
        // Start Color
        {
            Vector3 startcolor = _curEmitter->GetStartColor();
            ImGui::Text("Start Color");
            ImGui::SameLine();
            bool result = ImGui::ColorEdit3("##Start Color", (float*)&startcolor);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;
            _curEmitter->SetStartColor(startcolor);
        }
        // End Color
        {
            Vector3 endcolor = _curEmitter->GetEndColor();
            ImGui::Text("End Color");
            ImGui::SameLine();
            bool result = ImGui::ColorEdit3("##End Color", (float*)&endcolor);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;
            _curEmitter->SetEndColor(endcolor);
        }
        // Start Alpha
        {
            float StartAlpha = _curEmitter->GetStartOpacity();
            ImGui::Text("Start Alpha");
            ImGui::SameLine();
            bool result = ImGui::SliderFloat("##Start Alpha", &StartAlpha, _uiAlphaMin, _uiAlphaMax);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;
            _curEmitter->SetStartOpacity(StartAlpha);
        }
        // End Alpha
        {
            float EndAlpha = _curEmitter->GetEndOpacity();
            ImGui::Text("End Alpha");
            ImGui::SameLine();
            bool result = ImGui::SliderFloat("##End Alpha", &EndAlpha, _uiAlphaMin, _uiAlphaMax);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;
            _curEmitter->SetEndOpacity(EndAlpha);
        }
    }

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // ── [Scale by Type] ─────────────────────────────────────────────────────
    {
        if (ParticleType::SPRITE == _curEmitter->_particleType)
        {
            // Scale by Velocity / Axis
            {
                bool scaleVelFlag = _curEmitter->GetScaleByVelocityFlag();
                ImGui::Text("Scale by Velocity");
                ImGui::SameLine();
                bool result = ImGui::Checkbox("##Scale by Velocity", &scaleVelFlag);
                if (false == isSomethingChanged)
                    if (true == result)
                        isSomethingChanged = result;
                _curEmitter->SetScaleByVelocityFlag(scaleVelFlag);

                if (false == scaleVelFlag)
                {
                    Vector3 temp = _curEmitter->GetParticleAxis();
                    temp.Normalize();
                    float axis[3] = {temp.x, temp.y, temp.z};
                    ImGui::Text("Particle Axis");
                    ImGui::SameLine();
                    bool result2 = ImGui::SliderFloat3("##Particle Axis", axis, -1.0f, 1.0f);
                    if (false == isSomethingChanged)
                        if (true == result2)
                            isSomethingChanged = result2;
                    _curEmitter->SetParticleAxis({axis[0], axis[1], axis[2]});
                }
            }

            // Start Scale (2D)
            {
                Vector4 startscale    = _curEmitter->GetStartScale();
                float   startScale[2] = {startscale.x, startscale.y};
                ImGui::Text("Start Scale");
                ImGui::SameLine();
                bool result = ImGui::InputFloat2("##Start Scale", (float*)&startScale);
                if (false == isSomethingChanged)
                    if (true == result)
                        isSomethingChanged = result;
                startscale.x = startScale[0];
                startscale.y = startScale[1];
                _curEmitter->SetStartScale(startscale);
            }

            // End Scale (2D)
            {
                Vector4 endscale    = _curEmitter->GetEndScale();
                float   endScale[2] = {endscale.x, endscale.y};
                ImGui::Text("End Scale");
                ImGui::SameLine();
                bool result = ImGui::InputFloat2("##End Scale", (float*)&endScale);
                if (false == isSomethingChanged)
                    if (true == result)
                        isSomethingChanged = result;
                endscale.x = endScale[0];
                endscale.y = endScale[1];
                _curEmitter->SetEndScale(endscale);
            }
        }

        if (ParticleType::MESH == _curEmitter->_particleType)
        {
            // Start Scale (3D)
            {
                Vector4 startscale    = _curEmitter->GetStartScale();
                float   startScale[3] = {startscale.x, startscale.y, startscale.z};
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
            }

            // End Scale (3D)
            {
                Vector4 endscale    = _curEmitter->GetEndScale();
                float   endScale[3] = {endscale.x, endscale.y, endscale.z};
                ImGui::Text("End Scale");
                ImGui::SameLine();
                bool result = ImGui::InputFloat3("##End Scale", (float*)&endScale);
                if (false == isSomethingChanged)
                    if (true == result)
                        isSomethingChanged = result;
                endscale.x = endScale[0];
                endscale.y = endScale[1];
                endscale.z = endScale[2];
                _curEmitter->SetEndScale(endscale);
            }
        }

        if (ParticleType::RIBBON == _curEmitter->_particleType)
        {
            // Start Scale (2D)
            {
                Vector4 startscale    = _curEmitter->GetStartScale();
                float   startScale[2] = {startscale.x, startscale.y};
                ImGui::Text("Start Scale");
                ImGui::SameLine();
                bool result = ImGui::InputFloat2("##Start Scale", (float*)&startScale);
                if (false == isSomethingChanged)
                    if (true == result)
                        isSomethingChanged = result;
                startscale.x = startScale[0];
                startscale.y = startScale[1];
                _curEmitter->SetStartScale(startscale);
            }

            // End Scale (2D)
            {
                Vector4 endscale    = _curEmitter->GetEndScale();
                float   endScale[2] = {endscale.x, endscale.y};
                ImGui::Text("End Scale");
                ImGui::SameLine();
                bool result = ImGui::InputFloat2("##End Scale", (float*)&endScale);
                if (false == isSomethingChanged)
                    if (true == result)
                        isSomethingChanged = result;
                endscale.x = endScale[0];
                endscale.y = endScale[1];
                _curEmitter->SetEndScale(endscale);
            }
        }
    }

    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.0f);

    // ── [Physics: Mass / Distribution / Forces] ─────────────────────────────
    {
        // Mass
        {
            float mass = _curEmitter->GetParticleMass();
            ImGui::Text("Particle Mass");
            ImGui::SameLine();
            bool result = ImGui::InputFloat("##Particle Mass", &mass);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;
            _curEmitter->SetParticleMass(mass);
        }

        // Start Distribution Offset
        {
            Vector3 offset = _curEmitter->GetParticleStartDistributionOffset();
            ImGui::Text("Start Distribution Offset");
            ImGui::SameLine();
            bool result = ImGui::InputFloat3("##Start Distribution Offset", (float*)&offset);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;
            _curEmitter->SetParticleStartDistributionOffset(offset);
        }

        // End Distribution Offset
        {
            Vector3 offset = _curEmitter->GetParticleEndDistributionOffset();
            ImGui::Text("End Distribution Offset");
            ImGui::SameLine();
            bool result = ImGui::InputFloat3("##End Distribution Offset", (float*)&offset);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;
            _curEmitter->SetParticleEndDistributionOffset(offset);
        }

        // Drag Force
        {
            Vector4 force = _curEmitter->GetDragForce();
            ImGui::Text("Drag Force");
            ImGui::SameLine();
            bool result = ImGui::SliderFloat4("##Drag Force", (float*)&force, _uiVelScaleMin, _uiVelScaleMax);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;
            _curEmitter->SetDragForce(force);
        }

        // Vortex Force
        {
            Vector4 force = _curEmitter->GetVortexForce();
            ImGui::Text("Vortex Force");
            ImGui::SameLine();
            bool result = ImGui::SliderFloat4("##Vortex Force", (float*)&force, _uiVelScaleMin, _uiVelScaleMax);
            if (false == isSomethingChanged)
                if (true == result)
                    isSomethingChanged = result;
            if (force.Length() <= 0)
                force = {0.1f, 0.1f, 0.1f};
            _curEmitter->SetVortexForce(force);
        }
    }

    // ── [Notify Editor Refresh] ─────────────────────────────────────────────
    {
        if (true == isSomethingChanged)
        {
            UmParticleManager->RefreshEditor();
        }
    }
}

// ──────────────────────────────────────────────────────────────────────────────
// Effect pane
// ──────────────────────────────────────────────────────────────────────────────
void EditorParticleEffectDetails::ShowEffectDetails()
{
    // Effect Name
    {
        std::string name = _curEffect->GetEffectName();
        ImGui::InputText("Effect Name", &name);
        _curEffect->SetEffectName(name);
    }

    // Effect Lifetime
    {
        float lifetime = _curEffect->GetLifetime();
        ImGui::Text("Effect Lifetime");
        ImGui::SameLine();
        ImGui::InputFloat("##Effect Lifetime", &lifetime);
        _curEffect->SetLifetime(lifetime);
    }
}
