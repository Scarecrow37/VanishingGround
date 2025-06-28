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
    bool isAddButtonPressed = ImGui::Button("Add New Emitter", {150, 80});
    ImGui::SameLine();
    {

        static int  shapeRow = -1;
        const char* shapeitems[6]     = {"Sphere      ", "Cube        ", "Cylinder    ",
                                    "Cone        ", "Torus       ", "Mesh Surface"};
        static int  shapeIdx = 0;

        ImGui::Text("Emission Shape");
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
        LocationShape locationType = (LocationShape)shapeIdx;

        static int  renderrow = -1;
        const char* renderitems[3]     = {"Sprite", "Mesh  ", "Ribbon"};
        
        static int  renderIdx = 0;

        ImGui::Text("Render Type");
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
        ParticleType particleType = (ParticleType)renderIdx;

        if (true == isAddButtonPressed)
        {
            auto emitter =
                UmParticleManager.RegisterEmitter(_curEffect, 100000, 1000, 20, locationType, {0, 0, 0}, particleType);
        }
    }




    bool isSomeoneChanged   = false;

    for (const auto effect : UmParticleManager.GetEffectList())
    {
        // 부모 노드: 기본 플래그 사용
        ImGuiTreeNodeFlags parent_flags = ImGuiTreeNodeFlags_OpenOnArrow;
        bool               parent_open  = ImGui::TreeNodeEx(effect->GetEffectName().c_str(), parent_flags);

        effect->SetPosition({0,0,30});
        bool isHovered      = ImGui::IsItemHovered();
        bool isMouseClicked = ImGui::IsMouseClicked(0);
        if (true == isHovered && true == isMouseClicked)
        {
            _editorParticleEffectDetails->SetCurrentEffect(effect);

        }
        

        if (parent_open)
        {
            // 자식 노드: Leaf 플래그 사용
            ImGuiTreeNodeFlags leaf_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            for (const auto& emitter : effect->GetEmitterList())
            {

                if (ImGui::TreeNodeEx(emitter->GetEmitterName().c_str(), leaf_flags))
                {

                    bool isHovered             = ImGui::IsItemHovered();
                    bool isMouseClicked = ImGui::IsMouseClicked(0);
                    if (true == isHovered && true == isMouseClicked)
                    {
                        _editorParticleEffectDetails->SetCurrentEmitter(emitter);
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

