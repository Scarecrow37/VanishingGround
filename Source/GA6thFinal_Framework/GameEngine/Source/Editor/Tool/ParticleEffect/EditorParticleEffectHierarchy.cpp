#include "pch.h"
#include "EditorParticleEffectHierarchy.h"
#include "Engine/GraphicsCore/ParticleEffect.h"
#include "Engine/GraphicsCore/ParticleEmitter.h"
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
    _curEffect                   = UmParticleManager.GetEffectList()[0];
}

void EditorParticleEffectHierarchy::OnEndGui()
{

}

void EditorParticleEffectHierarchy::OnPreFrameBegin()
{
}

void EditorParticleEffectHierarchy::OnPostFrameBegin()
{
    LocationShape locationType;
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
            UmParticleManager.RegisterEmitter(_curEffect, 100000, 1000, 20, locationType, {0, 0, 0}, particleType);
    }


    bool isSomeoneChanged   = false;


    ImGui::Separator();

    ParticleEffect* effect = UmParticleManager.GetCurrentEditorEffect();
    {
        // 부모 노드: 기본 플래그 사용
        ImGuiTreeNodeFlags parent_flags = ImGuiTreeNodeFlags_OpenOnArrow;
        bool               parent_open  = ImGui::TreeNodeEx(effect->GetEffectName().c_str(), parent_flags);

        effect->SetPosition({0,0,0});
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
                            UmParticleManager.RefreshEditor();
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

