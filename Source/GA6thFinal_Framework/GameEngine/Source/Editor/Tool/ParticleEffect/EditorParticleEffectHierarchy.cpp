#include "pch.h"
#include "EditorParticleEffectHierarchy.h"

 EditorParticleEffectHierarchy::EditorParticleEffectHierarchy() 
 {
     SetLabel("Hierarchy##particleeffect");
     SetDockLayout(ImGuiDir_Left);
 }

 EditorParticleEffectHierarchy::~EditorParticleEffectHierarchy() {}

 void EditorParticleEffectHierarchy::OnTickGui()
{

 }

void EditorParticleEffectHierarchy::OnStartGui()
{
    auto&             system    = Global::editorModule->GetDockWindowSystem();
    EditorDockWindow* modelDock = system.GetDockWindow("EffectDock");
    _editorParticleEffectDetails         = modelDock->GetGui<EditorParticleEffectDetails>();
}

void EditorParticleEffectHierarchy::OnEndGui()
{

}

void EditorParticleEffectHierarchy::OnPreFrameBegin()
{
}

void EditorParticleEffectHierarchy::OnPostFrameBegin()
{

    bool            isnewbuttonpressed = ImGui::Button("New", {180, 50});
     if (true == isnewbuttonpressed)
    {
         auto newEffect = UmParticleManager->RegisterEffect();
         newEffect->SetLifetime(10.f);
         newEffect->SetEffectName("newEffect");
        UmParticleManager->SetCurrentEditorEffect(newEffect);
        _editorParticleEffectDetails->SetCurrentEffect(newEffect);
        _curEffect = newEffect;
    }

    ImGui::SameLine();

    bool isloadbuttonpressed = ImGui::Button("Load", {180, 50});
    bool isControlOPressed   = ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_O,false);
    if (true == isloadbuttonpressed || true == isControlOPressed)
    {
        HWND                    owner = UmApplication.GetHwnd();
        LPCWSTR                 title = L"Load vfx file";
        std::vector<File::Path> out;
        if (File::ShowOpenFileDialog(owner, title, L"", {{L"\0", L"*.vfx*\0"}}, false, out))
        {
            auto effect = UmParticleSerializer.Deserialize(out.front());
            UmParticleManager->SetCurrentEditorEffect(effect);
            _editorParticleEffectDetails->SetCurrentEffect(effect);
            _curEffect = effect;
        }
    }

    ParticleEffect* effect = UmParticleManager->GetCurrentEditorEffect();
    if (nullptr != effect)
    {
        bool isSaveButtonPressed = ImGui::Button("Save", {180, 50});
        bool isControlSPressed =
            ImGui::IsKeyDown(ImGuiKey::ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S, false);

        if (true == isSaveButtonPressed || true == isControlSPressed)
        {
            File::Path   path;
            std::wstring filename;
            if (File::ShowSaveFileDialog(UmApplication.GetHwnd(), L"Save as vfx file", L"", L"Effect.vfx", {}, path))
            {
                UmParticleSerializer.Serialize(_curEffect,path);
            }
        }
    }



    bool isrefreshbutton = ImGui::Button("refresh", {100, 30});
    if (true == isrefreshbutton)
    {
        UmParticleManager->RefreshEditor();

    }

    ImGui::SameLine();

    bool isAutorefresh = UmParticleManager->GetAutoRefresh();
    ImGui::Checkbox("Auto Refresh", &isAutorefresh);
    UmParticleManager->SetAutoRefresh(isAutorefresh);

    float deltaScale = UmParticleManager->GetDeltaScale();
    ImGui::SliderFloat("Time Speed",&deltaScale, 0.f,2.f);
    UmParticleManager->SetDeltaScale(deltaScale);





    ImGui::Text("current particle count : %d", UmParticleManager->GetTotalCount());
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.f);
    if (nullptr == effect)
    {
        return;
    }



    LocationShape   locationType;
    ParticleType  particleType;

    ImGui::BeginGroup();
    //location combobox
    {
        static int  shapeRow      = -1;
        const char* shapeitems[6] = {"Sphere      ", "Cube        ", "Cylinder    ",
                                     "Cone        ", "Torus       ", "Mesh Surface"};
        static int  shapeIdx      = 0;
        ImGui::Text("Emission Shape");
        ImGui::SetNextItemWidth(130);
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Emission Shape", shapeitems[shapeIdx]))
        {
            for (int n = 0; n < 6; n++)
            {
                bool is_selected = (shapeIdx == n);
                if (ImGui::Selectable(shapeitems[n], is_selected))
                {
                    shapeIdx = n;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        locationType = (LocationShape)shapeIdx;
    }
    //particleType combobox
    {

        static int  renderrow      = -1;
        const char* renderitems[3] = {"Sprite", "Mesh  ", "Ribbon"};
        static int  renderIdx      = 0;
        ImGui::Text("Render Type    ");
        ImGui::SetNextItemWidth(130);
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Render Type", renderitems[renderIdx]))
        {
            for (int n = 0; n < 3; n++)
            {
                bool is_selected = (renderIdx == n);
                if (ImGui::Selectable(renderitems[n], is_selected))
                {
                    renderIdx = n;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        particleType = (ParticleType)renderIdx;
    }
    ImGui::EndGroup();
    ImGui::SameLine();
    bool isAddButtonPressed = ImGui::Button("Add new Emitter", {180, ImGui::GetFrameHeight() * 2.f});


    if (true == isAddButtonPressed)
    {
        auto emitter =
            UmParticleManager->RegisterEmitter(_curEffect, 100000, 1000, 20, locationType, {0, 0, 0}, particleType);
    }
    bool isSomeoneChanged   = false;
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal,2.f);
    {
        // 부모 노드: 기본 플래그 사용
        ImGuiTreeNodeFlags parent_flags = ImGuiTreeNodeFlags_OpenOnArrow;
        bool               parent_open  = ImGui::TreeNodeEx(effect->GetEffectName().c_str(), parent_flags);

        effect->SetPosition({0,10,0});
        bool isHovered      = ImGui::IsItemHovered();
        bool isMouseClicked = ImGui::IsMouseClicked(0);
        if (true == isHovered && true == isMouseClicked)
        {
            _editorParticleEffectDetails->SetCurrentEffect(effect);

        }
        if (parent_open)
        {
            ImGui::GetStyle().ItemSpacing.y = 3.f; // 모든 위젯 사이의 기본 세로 간격을 10으로

            // 자식 노드: Leaf 플래그 사용
            ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            for (const auto& emitter : effect->GetEmitterList())
            {

                if (ImGui::TreeNodeEx(emitter->GetEmitterName().c_str(), leaf_flags))
                {

                    bool isHovered      = ImGui::IsItemHovered();
                    bool isMouseClicked = ImGui::IsMouseClicked(0);
                    if (true == isHovered && true == isMouseClicked)
                    {
                        _editorParticleEffectDetails->SetCurrentEmitter(emitter);
                        _curEmitter = emitter;
                    }
                    if (emitter == _curEmitter)
                    {

                        ImGui::SameLine();
                        ImVec2 buttonSize(120.0f, 25.0f);                // 버튼 크기 지정
                        float  avail = ImGui::GetContentRegionAvail().x; // 현재 남은 가로 공간

                        // 커서를 오른쪽 끝으로 이동 (버튼 너비만큼 빼고)
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + avail - buttonSize.x);

                        bool isRemoveButtonPressed = ImGui::Button("Remove Emitter", buttonSize);
                        if (true == isRemoveButtonPressed)
                        {
                            effect->RemoveEmitter(emitter);
                            _editorParticleEffectDetails->SetCurrentEmitter(nullptr);
                            UmParticleManager->RefreshEditor();
                        }
                    }
                }
            }
            ImGui::TreePop();
        }
    }
}

void EditorParticleEffectHierarchy::OnFrameClipped() {

}

void EditorParticleEffectHierarchy::OnFrameEnd()
{

}

void EditorParticleEffectHierarchy::OnFrameFocusEnter()
{
}

void EditorParticleEffectHierarchy::OnFrameFocusStay()
{

}

void EditorParticleEffectHierarchy::OnFrameFocusExit()
{

}

void EditorParticleEffectHierarchy::OnFramePopupOpened()
{

}

void EditorParticleEffectHierarchy::Serialize(std::string filepath) 
{
    
}

void EditorParticleEffectHierarchy::Deserialize(const std::string& filepath)
{
    std::ifstream is(filepath, std::ios::binary);
    if (!is.is_open())
        return;

    uint32_t nameLen = 0;
    is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    std::string effectname(nameLen, '\0');
    is.read(&effectname[0], nameLen);

    // lifetime
    float lifetime = 0.f;
    is.read(reinterpret_cast<char*>(&lifetime), sizeof(lifetime));

    auto newEffect = UmParticleManager->RegisterEffect();
    newEffect->SetLifetime(lifetime);
    newEffect->SetEffectName(effectname);

    uint32_t emitterCount = 0;
    is.read(reinterpret_cast<char*>(&emitterCount), sizeof(emitterCount));

    for (uint32_t i = 0; i < emitterCount; ++i)
    {

        uint32_t nameLen = 0;
        is.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        std::string emitterName(nameLen, '\0');
        is.read(&emitterName[0], nameLen);

        Vector3           emitterPosition;
        Vector3           emitterRotationE;
        Quaternion        emitterRotationQ;
        LocationShape     locationType;
        Vector3           locatorFactor;
        VelocityScaleType velocityType;
        Vector3           velocityFactor;
        float             emitterLifetime;
        float             particleLifetime;
        float             maxParticles;
        float             emissionRate;
        float             startDelay;
        float             spawnBurstFlag;
        float             spawnBurstCount;
        Vector3           startColor;
        float             startOpacity;
        Vector3           endColor;
        float             endOpacity;
        Vector4           startScale;
        Vector4           endScale;
        float             particleMass;
        Vector3           distributionOffset;
        Vector4           dragPoint;
        Vector4           dragForce;
        ParticleType      particleType;

        is.read(reinterpret_cast<char*>(&emitterPosition), sizeof(emitterPosition));
        is.read(reinterpret_cast<char*>(&emitterRotationE), sizeof(emitterRotationE));
        is.read(reinterpret_cast<char*>(&emitterRotationQ), sizeof(emitterRotationQ));
        is.read(reinterpret_cast<char*>(&locationType), sizeof(locationType));
        is.read(reinterpret_cast<char*>(&locatorFactor), sizeof(locatorFactor));
        is.read(reinterpret_cast<char*>(&velocityType), sizeof(velocityType));
        is.read(reinterpret_cast<char*>(&velocityFactor), sizeof(velocityFactor));
        is.read(reinterpret_cast<char*>(&emitterLifetime), sizeof(emitterLifetime));
        is.read(reinterpret_cast<char*>(&particleLifetime), sizeof(particleLifetime));
        is.read(reinterpret_cast<char*>(&maxParticles), sizeof(maxParticles));
        is.read(reinterpret_cast<char*>(&emissionRate), sizeof(emissionRate));
        is.read(reinterpret_cast<char*>(&startDelay), sizeof(startDelay));
        is.read(reinterpret_cast<char*>(&spawnBurstFlag), sizeof(spawnBurstFlag));
        is.read(reinterpret_cast<char*>(&spawnBurstCount), sizeof(spawnBurstCount));
        is.read(reinterpret_cast<char*>(&startColor), sizeof(startColor));
        is.read(reinterpret_cast<char*>(&startOpacity), sizeof(startOpacity));
        is.read(reinterpret_cast<char*>(&endColor), sizeof(endColor));
        is.read(reinterpret_cast<char*>(&endOpacity), sizeof(endOpacity));
        is.read(reinterpret_cast<char*>(&startScale), sizeof(startScale));
        is.read(reinterpret_cast<char*>(&endScale), sizeof(endScale));
        is.read(reinterpret_cast<char*>(&particleMass), sizeof(particleMass));
        is.read(reinterpret_cast<char*>(&distributionOffset), sizeof(distributionOffset));
        is.read(reinterpret_cast<char*>(&dragPoint), sizeof(dragPoint));
        is.read(reinterpret_cast<char*>(&dragForce), sizeof(dragForce));
        is.read(reinterpret_cast<char*>(&particleType), sizeof(particleType));

        // texture path
        uint32_t pathnameLen = 0;
        is.read(reinterpret_cast<char*>(&pathnameLen), sizeof(pathnameLen));
        std::string utf8Path(pathnameLen, '\0');
        is.read(&utf8Path[0], pathnameLen);
        int wideSize = MultiByteToWideChar(CP_UTF8, 0, utf8Path.data(), static_cast<int>(utf8Path.size()), nullptr, 0);
        std::wstring modelTexturePath(wideSize, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8Path.data(), static_cast<int>(utf8Path.size()), modelTexturePath.data(),
                            wideSize);

        Vector4 frameInfo{};
        if (particleType == ParticleType::SPRITE)
        {
            is.read(reinterpret_cast<char*>(&frameInfo), sizeof(frameInfo));
        }
        {
            auto emitter =
                UmParticleManager->RegisterEmitter(newEffect, maxParticles, emissionRate, emitterLifetime, locationType,
                                                  locatorFactor, particleType, modelTexturePath);
            emitter->SetEmitterName(emitterName);
            emitter->SetEmitterPosition(emitterPosition);
            emitter->SetEmitterRotationE(emitterRotationE);
            emitter->SetEmitterRotationQ(emitterRotationQ);
            emitter->SetVelocityType(velocityType);
            emitter->SetVelocityFactor(velocityFactor);
            emitter->SetParticleLifetime(particleLifetime);
            emitter->SetStartDelay(startDelay);
            emitter->SetSpawnBurstFlag(spawnBurstFlag);
            emitter->SetSpawnBurstCount(spawnBurstCount);
            emitter->SetStartColor(startColor);
            emitter->SetStartOpacity(startOpacity);
            emitter->SetEndColor(endColor);
            emitter->SetEndOpacity(endOpacity);
            emitter->SetStartScale(startScale);
            emitter->SetEndScale(endScale);
            emitter->SetParticleMass(particleMass);
            emitter->SetParticleDistributionOffset(distributionOffset);
            emitter->SetDragPoint(dragPoint);
            emitter->SetDragForce(dragForce);
        }
    }


    is.close();
}