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

}

void EditorParticleEffectHierarchy::OnFrameClipped()
{

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
