#include "pchScripts.h"
#include "QTESystem.h"
#include <QTE/Editor/QTEEditor.h>
#include <QTE/Track/QTETrack.h>

QTESystem::QTESystem() 
{
    // 전역 인스턴스는 하나만 존재해야 합니다.
    assert(_staticInstance == nullptr, "QTESystem is already exist in scene");
    _staticInstance = this;
}

QTESystem::~QTESystem() 
{
    // 전역 인스턴스는 하나만 존재해야 합니다.
    assert(_staticInstance == this, "QTESystem instance is not this");
    _staticInstance = nullptr;
}

void QTESystem::SerializedReflectEvent() {}

void QTESystem::DeserializedReflectEvent() {}

void QTESystem::ImGuiDrawPropertysEvent()
{
    if (ImGui::Button("Open QTE Editor"))
    {
        GetEditor().Open();
    }
    GetEditor().Show();
}

QTEEditor& QTESystem::GetEditor()
{
    static QTEEditor editor;
    return editor;
}