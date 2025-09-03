#include "pchScripts.h"
#include "QTESystem.h"
#include <QTE/Editor/QTEEditor.h>
#include <QTE/Track/QTETrack.h>

QTESystem::QTESystem() 
{
}

QTESystem::~QTESystem() {}

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